#include "stat_semantics/include/TokenStack.hpp"

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
    while (!s.empty()) 
    {
        if (s.top() == tk_instance)
            return s.size() - 1;
        
        s.pop();
    }

    return -1;
}