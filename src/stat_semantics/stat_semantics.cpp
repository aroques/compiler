#include <iostream>

#include "stat_semantics/include/stat_semantics.hpp"
#include "stat_semantics/include/TokenStack.hpp"
#include "error_handling/include/error_handling.hpp"

static void traverse_preorder(Node* node, TokenStack& stack, std::stack<int>& var_cnt_stack);
static void process_node(Node* node, TokenStack& stack, std::stack<int>& var_cnt_stack);
static void process_node_tokens(Node* node, TokenStack& tk_stack, std::stack<int>& var_cnt_stack);
static void verify_id_tk_definition(Token tk, TokenStack& tk_stack, std::stack<int>& var_cnt_stack);
static void verify_id_tk_usage(Token tk, TokenStack& tk_stack, std::stack<int>& var_cnt_stack);
static void print_error_and_exit(int line_no, std::string reason);
static void postprocess_node(Node* node, TokenStack& stack, std::stack<int>& var_cnt_stack);

bool verify_semantics(Node* root)
{
    TokenStack tk_stack;                         // Holds token definitions
    std::stack<int> var_cnt_stack ({0});    // Holds number of identifiers defined in each block. Top of stack holds number of variables defined in the current block.
    
    std::cout << "\nchecking static semantics..." << std::endl;

    traverse_preorder(root, tk_stack, var_cnt_stack);

    std::cout << "static semantics OK" << std::endl;
    
    return true;
}

static void traverse_preorder(Node* node, TokenStack& tk_stack, std::stack<int>& var_cnt_stack)
{
    if (node == NULL) 
        return;

    process_node(node, tk_stack, var_cnt_stack);

    if (node->children.size() > 0)
    {
        // recursively descend down tree
        for (auto child: node->children) 
            traverse_preorder(child, tk_stack, var_cnt_stack);
    }
    
    postprocess_node(node, tk_stack, var_cnt_stack);
    
    return;
}

static void process_node(Node* node, TokenStack& tk_stack, std::stack<int>& var_cnt_stack)
{
    if (node->label == "block")
        // entering new block, so start new var count
        var_cnt_stack.push(0);

    if (node->tokens.size() == 0)
        // no tokens to process
        return;

    process_node_tokens(node, tk_stack, var_cnt_stack);
}

static void process_node_tokens(Node* node, TokenStack& tk_stack, std::stack<int>& var_cnt_stack)
{
    for (auto tk: node->tokens)
    {
        if (tk.type != IDENTIFIER_TK)
            continue;
        
        // process identifier token
        if (node->label == "vars")
            verify_id_tk_definition(tk, tk_stack, var_cnt_stack);
        else 
            verify_id_tk_usage(tk, tk_stack, var_cnt_stack); 
    }
}

static void verify_id_tk_definition(Token tk, TokenStack& tk_stack, std::stack<int>& var_cnt_stack)
{
    int tk_idx = tk_stack.find(tk);

    if (var_cnt_stack.top() > 0 && tk_idx >= 0 && tk_idx < var_cnt_stack.top())
        print_error_and_exit(tk.line_number, "'" + tk.instance + "' is already defined in this block");
    
    // token not previously defined, so push onto stack and count it
    tk_stack.push(tk);
    var_cnt_stack.top()++;
}

static void verify_id_tk_usage(Token tk, TokenStack& tk_stack, std::stack<int>& var_cnt_stack)
{
    if (tk_stack.find(tk) < 0)
        print_error_and_exit(tk.line_number, "'" + tk.instance + "' has not been defined");
}

static void print_error_and_exit(int line_no, std::string reason)
{
    print_error_and_exit("semantics error: line " + std::to_string(line_no) + ": " + reason);
}

static void postprocess_node(Node* node, TokenStack& tk_stack, std::stack<int>& var_cnt_stack)
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