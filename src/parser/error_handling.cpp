#include "error_handling/include/error_handling.hpp"

static std::string get_error_reason(std::string expected_tk, std::string received_tk, int line_no)
{
    return "parser error: line " + std::to_string(line_no) + 
        ": expecting " + expected_tk + 
        ", but received " + received_tk;
}

void print_error_and_exit(std::string expected_token, std::string actual_token, int line_no)
{
    std::string error_reason = get_error_reason(expected_token, actual_token, line_no); 
    print_error_and_exit(error_reason);
}
