#include <iostream>

#include "stat_semantics/include/ParseTreeNodeProcessor.hpp"

static void traverse_preorder(Node* node, ParseTreeNodeProcessor& np);

std::string generate_target(Node* root)
{
    ParseTreeNodeProcessor np;

    std::cout << "\nprocessing parse tree..." << std::endl;

    traverse_preorder(root, np);

    std::cout << "parse tree processed" << std::endl;
    
    return np.get_target();
}

static void traverse_preorder(Node* node, ParseTreeNodeProcessor& np)
{
    if (node == NULL) 
        return;

    np.process_node(node);

    if (node->children.size() > 0)
    {
        // recursively descend down tree
        for (auto child: node->children) 
            traverse_preorder(child, np);
    }
    
    np.postprocess_node(node);
    
    return;
}