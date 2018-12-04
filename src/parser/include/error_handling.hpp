#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include <string>

void print_error_and_exit(std::string expected_token, std::string actual_token, int line_no);

#endif // !ERROR_HANDLING_HPP