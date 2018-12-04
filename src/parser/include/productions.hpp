#ifndef PRODUCTIONS_HPP
#define PRODUCTIONS_HPP

#include "scanner/include/token.hpp"
#include "parser/include/Node.hpp"

Node* program(Token& token);
Node* vars(Token& token);
Node* block(Token& token);
Node* stats(Token& token);
Node* mStat(Token& token);
Node* stat(Token& token);
Node* in(Token& token);
Node* out(Token& token);
Node* expr(Token& token);
Node* A(Token& token);
Node* M(Token& token);
Node* R(Token& token);
Node* if_(Token& token);
Node* loop(Token& token);
Node* conditionalStat(Token& token);
Node* RO(Token& token);
Node* assign(Token& token);

#endif // !PRODUCTIONS_HPP