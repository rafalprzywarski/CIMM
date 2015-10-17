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

}
