#include <iostream>
#include <fstream>

#include "parser/include/parser.hpp"
#include "code_gen/include/code_gen.hpp"

// Prototypes
void validate_argc(int argc);
void open_file(std::ifstream& file, char* argv[]);
std::string get_target_filename(int argc, char* argv[]);
void write_target_file(std::string target_filename, std::string target);

int main(int argc, char* argv[])
{
    validate_argc(argc);

    std::ifstream file;

    if (argc == 2) // Filename provided in argv[1]
    {
        open_file(file, argv);
        // Set cin to read from file
        std::cin.rdbuf(file.rdbuf());
    }

    Node* root = parser();
    std::string target = generate_target(root);

    std::string target_filename = get_target_filename(argc, argv);
    
    write_target_file(target_filename, target);

    if (file.is_open()) file.close();

    return 0;
}

void write_target_file(std::string target_filename, std::string target)
{
    std::ofstream out(target_filename);
    out << target;
    out.close();
}

void validate_argc(int argc)
{
    if (argc > 2)
    {
        std::cout << "Too many arguments." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void open_file(std::ifstream& file, char* argv[])
{
    std::string filename = argv[1];
    filename += ".fs182";
    file.open(filename);

    if (!file.is_open())
    {
        std::cout << "Failed to open file." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

std::string get_target_filename(int argc, char* argv[])
{
    std::string filename;
    
    if (argc == 2)
        filename = argv[1];
    else 
        filename = "out";
    
    return filename + ".asm";
}