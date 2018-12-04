#include "FSAState.hpp"

// Initialize static members
std::vector<std::array<int, NUM_COLUMNS>> FSAState::table = get_FSA_table();
std::set<int> FSAState::passive_states = {LEADING_WHITESPACE, INSIDE_COMMENT};

FSAState::FSAState() {};

FSAState::FSAState(int state)
{
    this->state = state;
}

int FSAState::get_state()
{
    return this->state;
}

bool FSAState::is_active()
{
    return passive_states.count(this->state) == 0;
}

bool FSAState::is_final()
{
    return this->state >= FINAL && this->state < ERROR;
}

bool FSAState::is_error()
{
    return this->state == ERROR;
}

FSAState FSAState::get_next_state(int state, char next_char)
{
    return FSAState(table[state][col_idx(next_char)]);
}