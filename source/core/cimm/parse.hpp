#pragma once
#include "expression.hpp"

namespace cimm
{

auto parse_expression(const string& expr_text) -> expression;

}
