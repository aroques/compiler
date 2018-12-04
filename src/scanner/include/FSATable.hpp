#ifndef FSATABLE_HPP
#define FSATABLE_HPP

#include <vector>
#include <array>

static const int NUM_COLUMNS = 8;

std::vector<std::array<int, NUM_COLUMNS>> get_FSA_table();
int col_idx(char c);

#endif // !FSATABLE_HPP