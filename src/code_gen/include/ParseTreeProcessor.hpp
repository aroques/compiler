#ifndef PARSE_TREE_NODE_PROCESSOR_HPP
#define PARSE_TREE_NODE_PROCESSOR_HPP

#include "code_gen/include/TokenStack.hpp"
#include "parser/include/Node.hpp"

#include <stack>

class ParseTreeProcessor
{
private:
    TokenStack tk_stack;        // Holds token definitions
    /*
        Holds number of identifiers defined in each block. 
        Top of stack holds the number of identifiers that 
        have been defined in the current (most recent) block.
    */
    std::stack<int> var_cnt_stack; // TODO: initialize to hold a zero. maybe initialization list({0});
    
    // Used to count number of temp vars and labels used in target language
    int temp_var_cnt = 0;
    int label_cnt = 0;

    std::string target = ""; // The target file that will be produced
    
    void postprocess_target();

    std::string get_temp_var();
    std::string get_label();

    void traverse_preorder(Node* node);
    void process_node(Node* node);
    void process_node_label(Node* node);

    void cond_stat(Node* node, std::string inlabel="");
    void eval_right_left(Node* left, Node* right, std::string op_tk_instance);

    // vars node helpers
    void verify_id_tk_definition(std::string tk_instance, int tk_line_no);
    void push_onto_stack(std::string tk_instance, std::string tk_val);

    void process_node_tokens(Node* node);
    void verify_id_tk_usage(Token tk);

public:
    ParseTreeProcessor();

    void process_parse_tree(Node* root);

    std::string get_target() { return target; };

};

#endif // !PARSE_TREE_NODE_PROCESSOR_HPP