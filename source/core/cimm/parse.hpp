#pragma once
#include "expression.hpp"

namespace cimm
{

struct parse_error : std::runtime_error
{
    parse_error(const string& msg) : std::runtime_error(msg) {}
};

auto parse_expression(const string& expr_text) -> expression;
auto parse_expressions(const string& expr_text) -> vector;

}
