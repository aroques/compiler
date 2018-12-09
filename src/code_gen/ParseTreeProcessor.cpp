#include "code_gen/include/ParseTreeProcessor.hpp"
#include "error_handling/include/error_handling.hpp"
#include "code_gen/include/asm_commands.hpp"
#include <iostream>

static void semantics_error(int line_no, std::string reason);

/*
    Public
*/
ParseTreeProcessor::ParseTreeProcessor() 
{
    // start off with 0 variables defined in global scope
    var_cnt_stack.push(0);
}

void ParseTreeProcessor::process_parse_tree(Node* root)
{
    traverse_preorder(root);
    
    postprocess_target();
    
    return;
}

/*
    Private
*/
void ParseTreeProcessor::postprocess_target()
{
    // initialize temp variables
    for (int i = 0; i < temp_var_cnt; i++)
        target += "V" + std::to_string(i) + " 0" + "\n";
    
    // add stop to end of file
    target += "STOP\n";
}

/*
    Getters for temp vars and labels
*/
std::string ParseTreeProcessor::get_temp_var()
{
    return "V" + std::to_string(temp_var_cnt++);
}

std::string ParseTreeProcessor::get_label()
{
    return "L" + std::to_string(label_cnt++);
}

/*
    Recursive preorder traversal
*/
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
    /*
        Variable definitions: vars
    */
    if (node->label == "vars")
    {
        // ID token definition
        Token id_tk = node->tokens.front();
        Token id_tk_val = node->tokens.at(1);
        
        verify_id_tk_definition(id_tk.instance, id_tk.line_number);
        
        push_onto_stack(id_tk.instance, id_tk_val.instance);

        // do not return here, so that we traverse children nodes
    }

    /*
        New blocks: block
    */
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
    
    /*
        Control flow: if and loop
    */
    if (node->label == "if")
    {
        cond_stat(node);
        
        return;
    }

    if (node->label == "loop")
    {
        std::string inlabel = get_label();
        target += inlabel + ": NOOP\n";

        cond_stat(node, inlabel);
        
        return;
    }

    /*
        Input and output: in and out
    */
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

    /*
        Variable assignment: assign
    */
    if (node->label == "assign")
    {
        // eval child expr
        traverse_preorder(node->children.front());
        Token tk = node->tokens.front();
        target += "STACKW " + std::to_string(tk_stack.find(tk.instance)) + "\n";
        
        return;
    }

    /*
        Expressions: 
            expr and it's possible children: A, M, and R
    */
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

    if (node->label == "R")
    {
        if (node->tokens.size() == 1)
        {   
            Token tk = node->tokens.front();

            if (tk.type == IDENTIFIER_TK)
                target += "STACKR " + std::to_string(tk_stack.find(tk.instance)) + "\n";
            else // token is number literal
                target += "LOAD " + tk.instance + "\n";
        }
        else // child node is expr
            traverse_preorder(node->children.front());
        
        return;
    }
    
    /*
        If we made it here and node has children, then call them
    */
    if (node->children.size() > 0)
    {
        // recursively descend down tree
        for (auto child: node->children) 
            traverse_preorder(child);
    }
}

void ParseTreeProcessor::cond_stat(Node* node, std::string inlabel)
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
    
    Node* stat_node = node->children.at(3);
    traverse_preorder(stat_node);

    // loop node adds un-conditional jump back to top inlabel to re-evaluate condition 
    if (inlabel != "")
        target += "BR " + inlabel + "\n";

    target += label + ": NOOP\n";   
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

/* 
    Vars node verifies id token definitions and pushes variables onto stack
*/
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
    target += "STACKW 0\n";
}

/*
    Rule that says identifiers must be defined before they are used
*/
void ParseTreeProcessor::verify_id_tk_usage(Token tk)
{
    if (tk_stack.find(tk.instance) < 0)
        semantics_error(tk.line_number, "'" + tk.instance + "' has not been defined");
}

static void semantics_error(int line_no, std::string reason)
{
    print_error_and_exit("semantics error: line " + std::to_string(line_no) + ": " + reason);
}