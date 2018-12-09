#include "code_gen/include/TokenStack.hpp"

TokenStack::TokenStack() {};

void TokenStack::push(std::string tk_instance)
{
    this->stack.push(tk_instance);
}

void TokenStack::pop()
{
    this->stack.pop();
}

int TokenStack::find(std::string tk_instance)
{
    std::stack<std::string> stack_to_search = this->stack;
    return find(stack_to_search, tk_instance);
}

int TokenStack::find(std::stack <std::string> s, std::string tk_instance) 
{
    int dist_from_top = 0;
    while (!s.empty()) 
    {
        if (s.top() == tk_instance)
            return dist_from_top;
        
        s.pop();
        dist_from_top++;
    }

    return -1;
}