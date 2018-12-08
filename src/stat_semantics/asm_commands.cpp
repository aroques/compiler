#include "error_handling/include/error_handling.hpp"
#include "stat_semantics/include/asm_commands.hpp"

std::string get_asm_cmd(std::string op_tk_instance)
{
    if (op_tk_instance == "+")
        return "ADD";
    
    if (op_tk_instance == "-")
        return "SUB";
    
    if (op_tk_instance == "*")
        return "MULT";
    
    if (op_tk_instance == "/")
        return "DIV";
    
    if (op_tk_instance == ">")
        return "BRZNEG";

    if (op_tk_instance == "<")
        return "BRZPOS";

    if (op_tk_instance == ">=")
        return "BRNEG";

    if (op_tk_instance == "<=")
        return "BRPOS";

    if (op_tk_instance == "==")
        return "BRZERO";

    print_error_and_exit("codegen: no assembly command for '" + op_tk_instance + "' operator");
    
    return "";
}