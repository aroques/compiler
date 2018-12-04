#include "stat_semantics/include/ParseTreeNodeProcessor.hpp"
#include "error_handling/include/error_handling.hpp"

static void print_error_and_exit(int line_no, std::string reason);

ParseTreeNodeProcessor::ParseTreeNodeProcessor() 
{
    var_cnt_stack.push(0);
}

void ParseTreeNodeProcessor::process_node(Node* node)
{
    if (node->label == "block")
        // entering new block, so start new var count
        var_cnt_stack.push(0);

    if (node->tokens.size() == 0)
        // no tokens to process
        return;

    process_node_tokens(node);
}

void ParseTreeNodeProcessor::process_node_tokens(Node* node)
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

void ParseTreeNodeProcessor::verify_id_tk_definition(Token tk)
{
    int tk_idx = tk_stack.find(tk);

    if (var_cnt_stack.top() > 0 && tk_idx >= 0 && tk_idx < var_cnt_stack.top())
        print_error_and_exit(tk.line_number, "'" + tk.instance + "' is already defined in this block");
    
    // token not previously defined, so push onto stack and count it
    tk_stack.push(tk);
    var_cnt_stack.top()++;
}

void ParseTreeNodeProcessor::verify_id_tk_usage(Token tk)
{
    if (tk_stack.find(tk) < 0)
        print_error_and_exit(tk.line_number, "'" + tk.instance + "' has not been defined");
}

static void print_error_and_exit(int line_no, std::string reason)
{
    print_error_and_exit("semantics error: line " + std::to_string(line_no) + ": " + reason);
}

void ParseTreeNodeProcessor::postprocess_node(Node* node)
{
    if (node->label == "block")
    {
        // leaving block 
        // so, pop token definitions
        for (int i = 0; i < var_cnt_stack.top(); i++)
            tk_stack.pop();
        
        // and pop number of variables defined in this block
        var_cnt_stack.pop();
    }
}
