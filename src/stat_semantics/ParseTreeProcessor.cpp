#include "stat_semantics/include/ParseTreeProcessor.hpp"
#include "error_handling/include/error_handling.hpp"

static void semantics_error(int line_no, std::string reason);
static std::string get_asm_cmd(std::string tk_instance);

ParseTreeProcessor::ParseTreeProcessor() 
{
    var_cnt_stack.push(0);
}

void ParseTreeProcessor::process_parse_tree(Node* root)
{
    traverse_preorder(root);
    postprocess_target();
    return;
}

void ParseTreeProcessor::postprocess_target()
{
    // initialize temp vars
    for (int i = 0; i < temp_var_cnt; i++)
        target += "V" + std::to_string(i) + " 0" + "\n";
    
    target += "STOP\n";
}

std::string ParseTreeProcessor::get_temp_var()
{
    return "V" + std::to_string(temp_var_cnt++);
}

void ParseTreeProcessor::traverse_preorder(Node* node)
{
    if (node == NULL) 
        return;

    process_node(node);

    // if (node->children.size() > 0)
    // {
    //     // recursively descend down tree
    //     for (auto child: node->children) 
    //         traverse_preorder(child);
    // }
    
    postprocess_node(node);
    
    return;
}

void ParseTreeProcessor::process_node(Node* node)
{
    process_node_label(node);

    if (node->tokens.size() == 0)
        // no tokens to process
        return;

    process_node_tokens(node);
}

void ParseTreeProcessor::process_node_label(Node* node)
{
    if (node->label == "vars")
    {
        // ID token definition
        Token id_tk = node->tokens.front();
        Token id_tk_val = node->tokens.at(1);
        
        verify_id_tk_definition(id_tk.instance, id_tk.line_number);
        
        push_onto_stack(id_tk.instance, id_tk_val.instance);

        if (node->children.size() > 0)
        {
            // recursively descend down tree
            for (auto child: node->children) 
                traverse_preorder(child);
        }

        return;
    }

    if (node->label == "block")
    {
        // entering new block, so start new var count
        var_cnt_stack.push(0);
        // TODO: Add PUSH to target
        if (node->children.size() > 0)
        {
            // recursively descend down tree
            for (auto child: node->children) 
                traverse_preorder(child);
        }

        return;
    }

    if (node->label == "in")
    {
        std::string temp_var = get_temp_var();
        target += "READ " + temp_var + "\n";
        target += "LOAD " + temp_var + "\n";
        std::string tk = node->tokens.front().instance;
        target += "STACKW " + std::to_string(tk_stack.find(tk)) + "\n";

        return;
    }

    if (node->label == "out")
    {
        traverse_preorder(node->children.front());
        std::string temp_var = get_temp_var();// process identifier token
        target += "STORE " + temp_var + "\n";
        target += "WRITE " + temp_var + "\n";

        return;
    }
    
    if (node->label == "if")
    {
        Node* right = node->children.at(2);  // expr
        Node* left = node->children.at(0);   // expr

        std::string temp_var = eval_right_left(right, left);

        


    }

    if (node->label == "expr")
    {
        if (node->tokens.size() == 1)
        {
            Node* right = node->children.at(1); // expr
            Node* left = node->children.at(0);  // A
            
            std::string temp_var = eval_right_left(right, left);

            Token tk = node->tokens.front();
            
            std::string asm_cmd = get_asm_cmd(tk.instance);
            target += asm_cmd + " " + temp_var + "\n";
        }
        else // evaluate child node A
            traverse_preorder(node->children.front());
        
        return;
    }

    if (node->label == "A")
    {
        if (node->tokens.size() == 1)
        {
            Node* right = node->children.at(1); // A
            Node* left = node->children.at(0);  // M

            std::string temp_var = eval_right_left(right, left);
            Token tk = node->tokens.front();
            
            std::string asm_cmd = get_asm_cmd(tk.instance);
            target += asm_cmd + " " + temp_var + "\n";

        }
        else // evaluate child node M
            traverse_preorder(node->children.front());
        
        return;
    }

    if (node->label == "R")
    {
        if (node->tokens.size() == 1)
        {   
            std::string tk = node->tokens.front().instance;
            target += "STACKR " + std::to_string(tk_stack.find(tk)) + "\n";
        }
        else // child node is expr
            traverse_preorder(node->children.front());
        
        return;
    }
    

    if (node->label == "M")
    {
        Node* child = node->children.front();
        
        if (child->label == "R")
            traverse_preorder(child);
        
        else // child is M, so negate what is in acc 
        {
            traverse_preorder(child);
            target += "MULT -1\n";
        }

        return;
    }

    if (node->children.size() > 0)
    {
        // recursively descend down tree
        for (auto child: node->children) 
            traverse_preorder(child);
    }
        
}

static std::string get_asm_cmd(std::string tk_instance)
{
    if (tk_instance == "+")
        return "ADD";
    
    if (tk_instance == "-")
        return "SUB";
    
    if (tk_instance == "*")
        return "MULT";
    
    if (tk_instance == "/")
        return "DIV";

    print_error_and_exit("codegen: no assembly command for '" + tk_instance + "' operator");
    
    return "";
}

std::string ParseTreeProcessor::eval_right_left(Node* right, Node* left)
{
    // evaluate right child
    traverse_preorder(right);
    
    // store result in temp var
    std::string temp_var = get_temp_var();
    target += "STORE " + temp_var + "\n";
    
    // evaluate left child 
    traverse_preorder(left);

    return temp_var;
}

void ParseTreeProcessor::process_node_tokens(Node* node)
{
    for (auto tk: node->tokens)
    {
        if (tk.type != IDENTIFIER_TK)
            continue;
        
        // process identifier token
        if (node->label != "vars")
            verify_id_tk_usage(tk); 
    }
}

void ParseTreeProcessor::verify_id_tk_definition(std::string tk_instance, int tk_line_no)
{
    int tk_idx = tk_stack.find(tk_instance);

    if (var_cnt_stack.top() > 0 && tk_idx >= 0 && tk_idx < var_cnt_stack.top())
        semantics_error(tk_line_no, "'" + tk_instance + "' is already defined in this block");
}

void ParseTreeProcessor::push_onto_stack(std::string tk_instance, std::string tk_val)
{
    // token not previously defined, so push onto stack and count it
    tk_stack.push(tk_instance);
    
    // TODO: Add push to target
    target += "LOAD " + tk_val + "\n";
    target += "PUSH\n";
    target += "STACKW " + std::to_string(var_cnt_stack.top()) + "\n";

    var_cnt_stack.top()++;
}

void ParseTreeProcessor::verify_id_tk_usage(Token tk)
{
    // TODO: Add STACKR or STACKW to target
    if (tk_stack.find(tk.instance) < 0)
        semantics_error(tk.line_number, "'" + tk.instance + "' has not been defined");
}

static void semantics_error(int line_no, std::string reason)
{
    print_error_and_exit("semantics error: line " + std::to_string(line_no) + ": " + reason);
}

void ParseTreeProcessor::postprocess_node(Node* node)
{
    if (node->label == "block")
    {
        // leaving block 
        // so, pop token definitions
        for (int i = 0; i < var_cnt_stack.top(); i++)
        {
            // TODO: Add POP to target
            tk_stack.pop();
            target += "POP\n";
        }
        
        // and pop number of variables defined in this block
        var_cnt_stack.pop();
    }

    // if (node->label == "M" && node->children.front()->label == "M")
    // {
    //     target += "MULT -1\n";
    // }


}