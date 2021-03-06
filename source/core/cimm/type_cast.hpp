#pragma once
#include "type_error.hpp"
#include "expression.hpp"

namespace cimm
{

auto as_list(const expression& e) -> list const&
{
    return boost::get<list>(e.value);
}

auto as_vector(const expression& e) -> vector const&
{
    if (auto val = boost::get<vector>(&e.value))
        return *val;
    throw type_error(e, "a vector");
}

auto as_symbol(const expression& e) -> symbol const&
{
    return boost::get<symbol>(e.value);
}

auto as_integer(const expression& e) -> integer
{
    if (auto val = boost::get<integer>(&e.value))
        return *val;
    throw type_error(e, "an integer");
}

auto is_error(const expression& e) -> bool
{
    return boost::get<error>(&e.value) != nullptr;
}

auto as_generic_method(expression& e) -> generic_method&
{
    return boost::get<generic_method>(e.value);
}

}
