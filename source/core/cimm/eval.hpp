#pragma once
#include "expression.hpp"

namespace cimm
{

inline std::string evaluate_expression(const expression& expr)
{
    return expr.value;
}

}
