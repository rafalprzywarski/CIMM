#pragma once
#include <string>

namespace cimm
{

using string = std::string;

inline auto pr_str(const string& s) -> string
{
    return '\"' + s + '\"';
}

}
