#include "stat_semantics/include/TokenStack.hpp"

TokenStack::TokenStack() {};

void TokenStack::push(Token tk)
{
    this->stack.push(tk);
}

void TokenStack::pop()
{
    this->stack.pop();
}

int TokenStack::find(Token tk)
{
    std::stack<Token> stack_to_search = this->stack;
    return find(stack_to_search, tk.instance);
}

int TokenStack::find(std::stack <Token> s, std::string id_instance) 
{
    int cnt = -1;

    while (!s.empty()) 
    {
        cnt++;

        if (s.top().instance == id_instance)
            return cnt;
        
        s.pop();
    }

    return -1;
}