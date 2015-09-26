#pragma once
#include "expression.hpp"
#include "str.hpp"

namespace cimm
{

struct evaluation_error : std::runtime_error
{
    evaluation_error(const string& msg) : std::runtime_error(msg) {}
};

struct cannot_create_symbol : evaluation_error
{
    cannot_create_symbol(const expression& e)
        : evaluation_error("cannot create a symbol from " + str(e)) { }
};

struct symbol_already_defined : evaluation_error
{
    symbol_already_defined(const symbol& s)
        : evaluation_error("symbol \'" + str(s) + "\' already defined") { }
};

struct undefined_symbol_error : evaluation_error
{
    undefined_symbol_error(const symbol& name)
        : evaluation_error("undefined symbol \'" + str(name) + "\'") { }
};

struct arity_error : evaluation_error
{
    arity_error(const string& msg)
        : evaluation_error(msg) { }
    arity_error(integer n, const string& from)
        : evaluation_error("Wrong number of args (" + str(n) + ") passed to: " + from) { }
};

}
