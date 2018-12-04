
#include "string/include/string.hpp"

/*
    Code adapted from:
        https://www.oreilly.com/library/view/c-cookbook/0596007612/ch04s09.html
*/
std::string join(const std::vector<std::string>& v, std::string c) 
{
    std::string out;

    for (std::vector<std::string>::const_iterator p = v.begin();
        p != v.end(); ++p) 
    {
        out += *p;
        
        if (p != v.end() - 1)
            out += c;
    }

    return out;
}