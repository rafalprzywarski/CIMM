#pragma once
#include "expression.hpp"

namespace cimm
{

inline auto str(const expression& e) -> string
{
    struct to_string : visitor<string>
    {
        string operator()(const std::string& s) const { return s; }
        string operator()(const integer& ) const { return {}; }
    };

    return apply(to_string(), e);
}

}
