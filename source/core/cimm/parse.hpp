#pragma once
#include "expression.hpp"

namespace cimm
{

inline expression parse_expression(const std::string& s)
{
    return {s};
}

}
