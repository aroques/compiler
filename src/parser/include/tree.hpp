#ifndef TREE_HPP
#define TREE_HPP

#include "parser/include/Node.hpp"

void print_level_order(struct Node* root);
void print_preorder(struct Node* node, int level = 0);

#endif // !TREE_HPP