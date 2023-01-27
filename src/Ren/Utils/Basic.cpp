#include "Ren/Utils/Basic.h"

using namespace Ren::Utils;

// See https://stackoverflow.com/questions/4983092/c-equivalent-of-sprintf for more details.
std::string Ren::Utils::string_format(std::string fmt, ...)
{
    int size=100;
    std::string str;
    va_list ap;

    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf(&str[0], size, fmt.c_str(), ap);
        va_end(ap);
   
        if (n > -1 && n < size) {
            str.resize(n); // Make sure there are no trailing zero char
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
}
