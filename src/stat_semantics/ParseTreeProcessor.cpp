#include "stat_semantics/include/ParseTreeProcessor.hpp"
#include "error_handling/include/error_handling.hpp"
#include <iostream>

static void semantics_error(int line_no, std::string reason);
static std::string get_asm_cmd(std::string op_tk_instance);

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

std::string ParseTreeProcessor::get_label()
{
    return "L" + std::to_string(label_cnt++);
}

void ParseTreeProcessor::traverse_preorder(Node* node)
{
    if (node == NULL) 
        return;

    process_node(node);
    
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

        // do not add return here to traverse children nodes
    }

    if (node->label == "block")
    {
        // entering new block, so start new var count
        var_cnt_stack.push(0);
        
        // recursively descend down tree
        for (auto child: node->children) 
            traverse_preorder(child);
        
        // Leaving block, so pop token definitions
        for (int i = 0; i < var_cnt_stack.top(); i++)
        {
            tk_stack.pop();
            target += "POP\n";
        }
        // and pop number of variables defined in this block
        var_cnt_stack.pop();

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
        // call child expr
        traverse_preorder(node->children.front());
        
        // store result in acc
        std::string temp_var = get_temp_var();
        target += "STORE " + temp_var + "\n";
        target += "WRITE " + temp_var + "\n";

        return;
    }
    
    if (node->label == "if")
    {
        Node* right = node->children.at(2);     // expr
        Node* left = node->children.at(0);      // expr
        Node* RO_node = node->children.at(1);   // relational operator node
        
        std::string op_tk_instance = RO_node->tokens.at(0).instance;
        
        if (RO_node->tokens.size() > 1)
            op_tk_instance += RO_node->tokens.at(1).instance;

        eval_right_left(right, left, "-"); // subtract right from left
        
        std::string label = get_label();
        
        if (op_tk_instance == "=")
        {
            // jump if acc positive or negative
            target += get_asm_cmd(">=") + " " + label + "\n";
            target += get_asm_cmd("<=") + " " + label + "\n";
        }
        else
            target += get_asm_cmd(op_tk_instance) + " " + label + "\n";
       
        traverse_preorder(node->children.at(3)); // call stat child
        target += label + ": NOOP\n";
        
        return;
    }

    if (node->label == "expr")
    {
        if (node->tokens.size() == 1)
        {
            Node* right = node->children.at(1); // expr
            Node* left = node->children.at(0);  // A
            Token tk = node->tokens.front();
            
            eval_right_left(right, left, tk.instance);
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
            Token tk = node->tokens.front();;

            eval_right_left(right, left, tk.instance);
        }
        else // evaluate child node M
            traverse_preorder(node->children.front());
        
        return;
    }

    if (node->label == "R")
    {
        if (node->tokens.size() == 1)
        {   
            Token tk = node->tokens.front();

            if (tk.type == IDENTIFIER_TK)
            {
                target += "STACKR " + std::to_string(tk_stack.find(tk.instance)) + "\n";
            }
            else // token is number literal
                target += "LOAD " + tk.instance + "\n";
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

void ParseTreeProcessor::eval_right_left(Node* right, Node* left, std::string op_tk_instance)
{
    // evaluate right childop_tk_instance
    traverse_preorder(right);
    
    // store result in temp var
    std::string temp_var = get_temp_var();
    target += "STORE " + temp_var + "\n";
    
    // evaluate left child 
    traverse_preorder(left);

    // get assembly command. e.g., ADD or MULT
    std::string asm_cmd = get_asm_cmd(op_tk_instance);
    
    target += asm_cmd + " " + temp_var + "\n";
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
    // push token onto stack and count it
    tk_stack.push(tk_instance);
    var_cnt_stack.top()++;
    
    target += "LOAD " + tk_val + "\n";
    target += "PUSH\n";
    target += "STACKW " + std::to_string(0) + "\n";
}

void ParseTreeProcessor::verify_id_tk_usage(Token tk)
{
    if (tk_stack.find(tk.instance) < 0)
        semantics_error(tk.line_number, "'" + tk.instance + "' has not been defined");
}

static void semantics_error(int line_no, std::string reason)
{
    print_error_and_exit("semantics error: line " + std::to_string(line_no) + ": " + reason);
}

static std::string get_asm_cmd(std::string op_tk_instance)
{
    if (op_tk_instance == "+")
        return "ADD";
    
    if (op_tk_instance == "-")
        return "SUB";
    
    if (op_tk_instance == "*")
        return "MULT";
    
    if (op_tk_instance == "/")
        return "DIV";
    
    if (op_tk_instance == ">")
        return "BRZNEG";

    if (op_tk_instance == "<")
        return "BRZPOS";

    if (op_tk_instance == ">=")
        return "BRNEG";

    if (op_tk_instance == "<=")
        return "BRPOS";

    if (op_tk_instance == "==")
        return "BRZERO";

    print_error_and_exit("codegen: no assembly command for '" + op_tk_instance + "' operator");
    
    return "";
}