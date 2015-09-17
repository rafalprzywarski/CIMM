#include "default_environment.hpp"

namespace cimm
{

namespace
{

auto add_integers(environment&, const list& l) -> expression
{
    integer sum{0};
    for (auto e = std::begin(l.value); e != std::end(l.value); ++e)
        sum += boost::get<integer>(e->value);
    return sum;
}

auto subtract_integers(environment&, const list& l) -> expression
{
    return boost::get<integer>(l.value.at(0).value) - boost::get<integer>(l.value.at(1).value);
}

auto is_equal(environment&, const list& l) -> expression
{
    auto e = std::begin(l.value);
    if (e == std::end(l.value))
      return true;
    auto first = *e;
    while (++e != std::end(l.value))
        if (first != *e)
            return false;
    return true;
}

}

auto create_default_environment() -> environment
{
    environment env;

    define_native_function(env, identifier("+"), add_integers);
    define_native_function(env, identifier("-"), subtract_integers);
    define_native_function(env, identifier("="), is_equal);

    return env;
}

}
