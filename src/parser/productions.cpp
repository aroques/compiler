#include <iostream>

#include "scanner/include/scanner.hpp"
#include "scanner/include/token.hpp"

#include "parser/include/productions.hpp"
#include "parser/include/error_handling.hpp"

Node* program(Token& token) 
{
    if (token.instance == "void") 
    { 
        Node* node = get_node("program");
        
        token = get_next_token(); 
        
        node->children.push_back(vars(token));
        node->children.push_back(block(token));
        
        return node; 
    }
    else print_error_and_exit(token_string(KEYWORD_TK, "void"), token_string(token), token.line_number);

    return NULL;
}

Node* vars(Token& token) 
{
    if (token.instance == "var") 
    {
        Node* node = get_node("vars");

        token = get_next_token(); 
        
        if (token.type == IDENTIFIER_TK)
        {
            node->tokens.push_back(token);

            token = get_next_token(); 
        
            if (token.instance == ":")
            {
                token = get_next_token(); 
        
                if (token.type == NUMBER_TK)
                {
                    node->tokens.push_back(token);
                    
                    token = get_next_token(); 
                    
                    node->children.push_back(vars(token));
                    
                    return node;
                } 
                else print_error_and_exit(token_string(NUMBER_TK, ""), token_string(token), token.line_number);
            }
            else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, ":"), token_string(token), token.line_number);
        }
        else print_error_and_exit(token_string(IDENTIFIER_TK, ""), token_string(token), token.line_number);
    }

    return NULL;
}

Node* block(Token& token) 
{
    if (token.instance == "start") 
    { 
        Node* node = get_node("block");
        
        token = get_next_token(); 
        
        node->children.push_back(vars(token));
        node->children.push_back(stats(token));

        if (token.instance == "stop")
        {
            token = get_next_token();
            return node;
        }
        else print_error_and_exit(token_string(KEYWORD_TK, "stop"), token_string(token), token.line_number);
    }
    else print_error_and_exit(token_string(KEYWORD_TK, "start"), token_string(token), token.line_number);
    
    return NULL;
}

Node* stats(Token& token) 
{
    Node* node = get_node("stats");
    
    node->children.push_back(stat(token));
    node->children.push_back(mStat(token));
    
    return node;
}

Node* mStat(Token& token) 
{
    std::set<std::string> stat_productions = 
        {"scan", "out", "start", "if", "loop", "let"};
    
    if (stat_productions.count(token.instance) > 0)
    {
        Node* node = get_node("mStat");
        
        node->children.push_back(stat(token));
        node->children.push_back(mStat(token));
        
        return node;
    }

    return NULL;
}

Node* stat(Token& token) 
{
    Node* node = get_node("stat");
    
    if (token.instance == "scan")
    {
        node->children.push_back(in(token));
        return node;
    }
    
    if (token.instance == "out") 
    {
        node->children.push_back(out(token));
        return node;
    }
    
    if (token.instance == "start")
    {
        node->children.push_back(block(token));
        return node;
    }  
    
    if (token.instance == "if")
    {
        node->children.push_back(if_(token));
        return node;
    }  
    
    if (token.instance == "loop")
    {
        node->children.push_back(loop(token));
        return node;
    }  
    
    if (token.instance == "let")
    {
        node->children.push_back(assign(token));
        return node;
    }  

    print_error_and_exit("scan, out, start, if, loop, or " + 
        token_string(KEYWORD_TK, "let"), token_string(token), token.line_number);
    
    return NULL;
}

Node* in(Token& token) 
{
    if (token.instance == "scan")
    {
        Node* node = get_node("in");

        token = get_next_token();

        if (token.type == IDENTIFIER_TK)
        {
            node->tokens.push_back(token);

            token = get_next_token();

            if (token.instance == ".")
            {
                token = get_next_token();
                return node;
            }
            else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, "."), token_string(token), token.line_number);
        }
        else print_error_and_exit(token_string(IDENTIFIER_TK, ""), token_string(token), token.line_number);
    }
    else print_error_and_exit(token_string(KEYWORD_TK, "scan"), token_string(token), token.line_number);

    return NULL;
}

Node* out(Token& token) 
{
    if (token.instance == "out")
    {
        Node* node = get_node("out");

        token = get_next_token();
        
        if (token.instance == "[")
        {
            token = get_next_token();
            
            node->children.push_back(expr(token));

            if (token.instance == "]")
            {
                token = get_next_token();
                
                if (token.instance == ".")
                {
                    token = get_next_token();
                    return node;
                }
                else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, "."), token_string(token), token.line_number);
            }
            else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, "]"), token_string(token), token.line_number);
        }
        else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, "["), token_string(token), token.line_number);
    }
    else print_error_and_exit(token_string(KEYWORD_TK, "out"), token_string(token), token.line_number);

    return NULL;
}

Node* expr(Token& token) 
{
    Node* node = get_node("expr");

    node->children.push_back(A(token));
    
    if (token.instance == "/" || token.instance == "*")
    {
        node->tokens.push_back(token);

        token = get_next_token();
        
        node->children.push_back(expr(token));
        
        return node;
    }

    return node;
}

Node* A(Token& token) 
{   
    Node* node = get_node("A");

    node->children.push_back(M(token));
    
    if (token.instance == "+" || token.instance == "-")
    {
        node->tokens.push_back(token);
        
        token = get_next_token();
        
        node->children.push_back(A(token));
        
        return node;
    }

    return node;
}

Node* M(Token& token) 
{   
    Node* node = get_node("M");

    if (token.instance == "-")
    {
        token = get_next_token();
    
        node->children.push_back(M(token));
        
        return node;
    }

    node->children.push_back(R(token));
    
    return node;
}

Node* R(Token& token) 
{   
    Node* node = get_node("R");

    if (token.instance == "(")
    {
        token = get_next_token();
        node->children.push_back(expr(token));

        if (token.instance == ")")
        {
            token = get_next_token();
            return node;
        }
        else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, ")"), token_string(token), token.line_number);
    }
    
    if (token.type == IDENTIFIER_TK || token.type == NUMBER_TK)
    {
        node->tokens.push_back(token);
        token = get_next_token();
        return node;
    }
    
    print_error_and_exit(token_string(IDENTIFIER_TK, "") + ", " + token_string(NUMBER_TK, "") + ", or "
        + token_string(OPERATOR_DELIMITER_TK, "("), token_string(token), token.line_number);
    
    return NULL;
}

Node* if_(Token& token) 
{   
    if (token.instance == "if")
    {
        Node* node = get_node("if");
        token = get_next_token();
        
        if (token.instance == "(")
        {
            token = get_next_token();
            
            node->children.push_back(expr(token));
            node->children.push_back(RO(token));
            node->children.push_back(expr(token));

            if (token.instance == ")")
            {
                token = get_next_token();
                node->children.push_back(stat(token));
                return node;
            }
            else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, ")"), token_string(token), token.line_number);
        }
        else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, "("), token_string(token), token.line_number);
    }
    else print_error_and_exit(token_string(KEYWORD_TK, "if"), token_string(token), token.line_number);

    return NULL;
}

Node* loop(Token& token) 
{   
    if (token.instance == "loop")
    {
        Node* node = get_node("loop");
        token = get_next_token();

        if (token.instance == "(")
        {
            token = get_next_token();
            
            node->children.push_back(expr(token));
            node->children.push_back(RO(token));
            node->children.push_back(expr(token));

            if (token.instance == ")")
            {
                token = get_next_token();
                node->children.push_back(stat(token));
                return node;
            }
            else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, ")"), token_string(token), token.line_number);
        }
        else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, "("), token_string(token), token.line_number);
    }
    else print_error_and_exit(token_string(KEYWORD_TK, "loop"), token_string(token), token.line_number);

    return NULL;
}

Node* RO(Token& token) 
{   
    if (token.instance == "<" || token.instance == ">" || token.instance == "=")
    {
        Node* node = get_node("RO");
        node->tokens.push_back(token);
        token = get_next_token();
        
        if (token.instance == "=")
        {
            node->tokens.push_back(token);
            token = get_next_token();
            return node;
        }

        return node;
    }

    print_error_and_exit("<, >, or " + 
        token_string(OPERATOR_DELIMITER_TK, "="), token_string(token), token.line_number);

    return NULL;
}

Node* assign(Token& token) 
{   
    if (token.instance == "let")
    {
        Node* node = get_node("assign");
        token = get_next_token();
        
        if (token.type == IDENTIFIER_TK)
        {
            node->tokens.push_back(token);
            token = get_next_token();

            if (token.instance == "=")
            {
                token = get_next_token();
                node->children.push_back(expr(token));
                
                if (token.instance == ".")
                {
                    token = get_next_token();
                    return node;
                }
                else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, "."), token_string(token), token.line_number);
            }
            else print_error_and_exit(token_string(OPERATOR_DELIMITER_TK, "="), token_string(token), token.line_number);
        }
        else print_error_and_exit(token_string(IDENTIFIER_TK, ""), token_string(token), token.line_number);
    }
    else print_error_and_exit(token_string(KEYWORD_TK, "let"), token_string(token), token.line_number);

    return NULL;
}