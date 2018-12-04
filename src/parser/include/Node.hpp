#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <vector>

#include "scanner/include/token.hpp"

struct Node
{
    std::string label;
    std::vector<struct Node*> children;
    std::vector<Token> tokens;
};

struct Node* get_node(std::string label);

#endif // !NODE_HPP