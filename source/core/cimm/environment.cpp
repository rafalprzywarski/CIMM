#include "environment.hpp"

namespace cimm
{

auto define_native_function(environment& env, identifier name, native_function fn) -> void
{
    env.functions.emplace(std::move(name), std::move(fn));
}

namespace
{

expression add_integers(environment&, const list& l)
{
    integer sum{0};
    auto e = std::begin(l.value);
    while (++e != std::end(l.value))
        sum += boost::get<integer>(e->value);
    return sum;
}

expression subtract_integers(environment&, const list& l)
{
    return boost::get<integer>(l.value.at(1).value) - boost::get<integer>(l.value.at(2).value);
}

}

environment create_environment()
{
    environment env;

    define_native_function(env, identifier("+"), add_integers);
    define_native_function(env, identifier("-"), subtract_integers);

    return env;
}

}