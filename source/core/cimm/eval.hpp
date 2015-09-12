#pragma once
#include "expression.hpp"
#include "environment.hpp"

namespace cimm
{

expression evaluate_expression(environment& env, const expression& expr);

}
