#include "FSATable.hpp"

std::vector<std::array<int, NUM_COLUMNS>> get_FSA_table()
{
    std::vector<std::array<int, NUM_COLUMNS>> table;
    table.push_back({1, 2, 3, 103, 0, 4, 200, 200});
    table.push_back({100, 100, 100, 100, 100, 100, 100, 100});
    table.push_back({101, 2, 2, 101, 101, 101, 2, 101});
    table.push_back({102, 102, 3, 102, 102, 102, 102, 102});
    table.push_back({4, 4, 4, 103, 4, 0, 4, 4});
    return table;
}

enum Column {OP_DELIM, LETTER, NUMBER, EOF_CHAR, WS, DOLLAR_SIGN, UNDERSCORE, NOT_IN_ALPHABET};

int col_idx(char c)
{
    std::string op_delim = "=<>:+-*/%%.(),{};[]";
    if (op_delim.find(c) != std::string::npos) return OP_DELIM; 
    
    if (c == '_') return UNDERSCORE;

    if (c == '$') return DOLLAR_SIGN;

    if (isspace(c)) return WS;

    if (isdigit(c)) return NUMBER;

    if (isalpha(c)) return LETTER;

    if (c == EOF) return EOF_CHAR;

    return NOT_IN_ALPHABET;
}