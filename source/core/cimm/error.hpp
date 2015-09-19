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

struct cannot_create_symbol : evaluation_error
{
    cannot_create_symbol(const expression& e)
        : evaluation_error("cannot create a symbol from " + str(e)) { }
};

}
