#pragma once
#include "expression.hpp"
#include "environment.hpp"

namespace cimm
{

inline auto evaluate_expression(environment& env, const expression& expr)
{
    return expr;
}

}
