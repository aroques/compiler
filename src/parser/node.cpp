#include "parser/include/Node.hpp"

struct Node* get_node(std::string label)
{
    struct Node* temp =  new struct Node();
    temp->label = label;
    return temp;
}
