#include <iostream>

void print_error_and_exit(std::string error_message)
{
    std::cout << error_message << std::endl;
    std::exit(EXIT_FAILURE);
}