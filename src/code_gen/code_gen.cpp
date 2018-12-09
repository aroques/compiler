#include <iostream>

#include "code_gen/include/ParseTreeProcessor.hpp"

std::string generate_target(Node* root)
{
    ParseTreeProcessor np;

    np.process_parse_tree(root);
    
    return np.get_target();
}