#ifndef TOKEN_STACK_HPP
#define TOKEN_STACK_HPP

#include "scanner/include/token.hpp"
#include <stack>

class TokenStack
{
private:
    std::stack<std::string> stack;
    int find(std::stack <std::string> s, std::string tk_instance);

public:
    TokenStack();

    void push(std::string tk_instance);
    void pop();
    int find(std::string tk_instance);
};

#endif // !TOKEN_STACK_HPP