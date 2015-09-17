#pragma once
#include "expression.hpp"
#include <unordered_map>

namespace cimm
{

struct environment;

using native_function = expression(*)(environment&, list const&);

struct environment
{
    std::unordered_map<string, native_function> functions;
};

auto define_native_function(environment& env, identifier name, native_function fn) -> void;

}
