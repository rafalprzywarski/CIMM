#pragma once
#include "expression.hpp"
#include "str.hpp"

namespace cimm
{

struct evaluation_error : std::runtime_error
{
    evaluation_error(const string& msg) : std::runtime_error(msg) {}
};

struct undefined_function_error : evaluation_error
{
    undefined_function_error(const symbol& name)
        : evaluation_error("undefined function \'" + str(name) + "\'") { }
};

}
