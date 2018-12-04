#ifndef TOKEN_STACK_HPP
#define TOKEN_STACK_HPP

#include "scanner/include/token.hpp"
#include <stack>

class TokenStack
{
private:
    std::stack<Token> stack;
    int find(std::stack <Token> s, std::string id_instance);

public:
    TokenStack();

    void push(Token tk);
    void pop();
    int find(Token tk);
};

#endif // !TOKEN_STACK_HPP