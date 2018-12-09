#include <iostream>

#include "code_gen/include/ParseTreeProcessor.hpp"

std::string generate_target(Node* root)
{
    ParseTreeProcessor np;

    std::cout << "\nprocessing parse tree..." << std::endl;

    np.process_parse_tree(root);

    std::cout << "parse tree processed" << std::endl;
    
    return np.get_target();
}