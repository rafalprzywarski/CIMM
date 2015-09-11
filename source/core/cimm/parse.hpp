#pragma once
#include "expression.hpp"

namespace cimm
{

inline auto parse_expression(const string& s) -> expression
{
    return {s};
}

}
