#pragma once
#include "expression.hpp"
#include <unordered_map>

namespace cimm
{

struct environment
{
    std::unordered_map<string, expression> definitions;
};

auto define_native_function(environment& env, native_function fn) -> void;

template <typename F>
auto define_native_function(environment& env, symbol name, F fn) -> void
{
    define_native_function(env, native_function{str(name), fn});
}

}
