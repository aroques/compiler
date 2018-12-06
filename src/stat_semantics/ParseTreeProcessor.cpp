#include "stat_semantics/include/ParseTreeProcessor.hpp"
#include "error_handling/include/error_handling.hpp"

static void print_error_and_exit(int line_no, std::string reason);

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
    for (int i = 0; i < temp_var_cnt; i++)
        target += "V" + std::to_string(i) + "\n";
    
    target += "STOP\n";
}

void ParseTreeProcessor::traverse_preorder(Node* node)
{
    if (node == NULL) 
        return;

    process_node(node);

    if (node->children.size() > 0)
    {
        // recursively descend down tree
        for (auto child: node->children) 
            traverse_preorder(child);
    }
    
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
    if (node->label == "block")
    {
        // entering new block, so start new var count
        var_cnt_stack.push(0);
        // TODO: Add PUSH to target
    }

    if (node->label == "in")
        target += "READ " + node->tokens.front().instance + "\n";

    if (node->label == "out")
    {
        target += "STORE V" + std::to_string(temp_var_cnt) + "\n";
        target += "WRITE V" + std::to_string(temp_var_cnt++) + "\n";
    }

    if (node->label == "R")
    {
        if (node->children.size() == 1)
            traverse_preorder(node->children.front());
        
        if (node->tokens.size() == 1)
            target += "LOAD " + node->tokens.front().instance + "\n";
    }

    if (node->label == "M")
    {
        if (node->children.front()->label == "R")
            traverse_preorder(node->children.front());
        
        else // child is M, so negate what is in acc 
        {
            traverse_preorder(node->children.front());
            target += " MULT -1\n";
        }
    }
}

void ParseTreeProcessor::process_node_tokens(Node* node)
{
    for (auto tk: node->tokens)
    {
        if (tk.type != IDENTIFIER_TK)
            continue;
        
        // process identifier token
        if (node->label == "vars")
            verify_id_tk_definition(tk);
        else 
            verify_id_tk_usage(tk); 
    }
}

void ParseTreeProcessor::verify_id_tk_definition(Token tk)
{
    int tk_idx = tk_stack.find(tk);
    // TODO: Add STACKR or STACKW to target

    if (var_cnt_stack.top() > 0 && tk_idx >= 0 && tk_idx < var_cnt_stack.top())
        print_error_and_exit(tk.line_number, "'" + tk.instance + "' is already defined in this block");
    
    // token not previously defined, so push onto stack and count it
    tk_stack.push(tk);
    // TODO: Add push to target
    var_cnt_stack.top()++;
}

void ParseTreeProcessor::verify_id_tk_usage(Token tk)
{
    // TODO: Add STACKR or STACKW to target
    if (tk_stack.find(tk) < 0)
        print_error_and_exit(tk.line_number, "'" + tk.instance + "' has not been defined");
}

static void print_error_and_exit(int line_no, std::string reason)
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
            tk_stack.pop();
            // TODO: Add POP to target
        
        // and pop number of variables defined in this block
        var_cnt_stack.pop();
    }
}