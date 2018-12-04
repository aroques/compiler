#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include "FSAState.hpp"

enum TokenType
{
    OPERATOR_DELIMITER_TK = FINAL,
    IDENTIFIER_TK,
    NUMBER_TK,
    END_OF_FILE_TK,
    KEYWORD_TK
};

struct Token
{
    int type;
    std::string instance;
    int line_number;
};

void print_token_header();
void print_token(Token t);
int get_token_type(int state, std::string token_instance);
std::string token_string(Token t);
std::string token_string(int token_type, std::string token_instance);
std::string tree_token(Token t);

#endif // !TOKEN_HPP