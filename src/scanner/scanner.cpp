#include <iostream>

#include "scanner.hpp"
#include "token.hpp"
#include "FSAState.hpp"

static char get_next_char();

Token get_next_token()
{
    static int line_number = 1;

    char next_char = get_next_char();

    FSAState state = FSAState(0), next_state;
    std::string token_instance = "";

    while (!state.is_final())
    {
        next_state = FSAState::get_next_state(int(state), next_char);
        if (next_state.is_error())
        {
            std::cout << "scanner error: no token starts with the '" 
                + std::string(1, next_char) + "' character. line: " 
                << line_number << std::endl;
            exit(EXIT_FAILURE);
        }
        
        if (next_state.is_final()) break;
        else
        {
            // Not final state
            state = next_state;

            if (state.is_active()) 
                token_instance += next_char; 
            
            // if current char is \n, then increment line number
            if (next_char == '\n') line_number++;

            next_char = get_next_char();
        }
    }

    std::cin.unget(); // unget look-ahead

    return Token {
        get_token_type(int(next_state), token_instance),
        token_instance,
        line_number  
    };
}

static char get_next_char()
{
    char c = EOF;
    
    std::cin.get(c);
    
    return c;
}