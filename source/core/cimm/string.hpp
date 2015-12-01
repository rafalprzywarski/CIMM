#pragma once
#include <string>

namespace cimm
{

using string = std::string;

inline auto pr_str(const string& s) -> string
{
    string printed = "\"";
    for (auto c : s)
        if (c == '\n')
            printed += "\\n";
        else if (c == '\"')
            printed += "\\\"";
        else
            printed += c;
    printed += "\"";
    return printed;
}

}
