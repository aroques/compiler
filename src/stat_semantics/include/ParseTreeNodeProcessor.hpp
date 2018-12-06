#ifndef PARSE_TREE_NODE_PROCESSOR_HPP
#define PARSE_TREE_NODE_PROCESSOR_HPP


#include "stat_semantics/include/TokenStack.hpp"
#include "parser/include/Node.hpp"

#include <stack>

class ParseTreeNodeProcessor
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
    bool target_delivered = 0;

    void process_node_label(Node* node);
    void process_node_tokens(Node* node);
    void verify_id_tk_definition(Token tk);
    void verify_id_tk_usage(Token tk);

public:
    ParseTreeNodeProcessor();

    void process_node(Node* node);
    void postprocess_node(Node* node);
    std::string get_target();

};

#endif // !PARSE_TREE_NODE_PROCESSOR_HPP