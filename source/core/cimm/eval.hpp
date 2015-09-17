#pragma once
#include "expression.hpp"
#include "environment.hpp"

namespace cimm
{

auto evaluate_expression(environment& env, const expression& expr) -> expression;

}
