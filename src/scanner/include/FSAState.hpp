#ifndef FSASTATE_HPP
#define FSASTATE_HPP

#include <set>
#include <vector>
#include "FSATable.hpp"

static const int FINAL = 100; // Final states
static const int ERROR = 200; // Error states

enum State
{
    LEADING_WHITESPACE = 0,
    INSIDE_COMMENT = 4
};

class FSAState
{
private:
    int state;
    
    static std::set<int>passive_states;
    static std::vector<std::array<int, NUM_COLUMNS>> table;

public:
    FSAState();
    FSAState(int state);
    operator int() { return state; }

    int get_state();
    bool is_active();
    bool is_final();
    bool is_error();

    static FSAState get_next_state(int state, char next_char);
};

#endif // !FSASTATE_HPP