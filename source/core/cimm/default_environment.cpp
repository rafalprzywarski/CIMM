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

auto is_unequal(environment& env, const list& l) -> expression
{
    return not boost::get<boolean>(is_equal(env, l).value);
}

auto not_f(environment&, const list& l) -> expression
{
    if (l.value.empty())
        return true;
    auto e = first(l).value;
    if (boost::get<nil_type>(&e))
        return true;
    if (auto b = boost::get<boolean>(&e))
        return not *b;
    return false;
}

}

auto create_default_environment() -> environment
{
    environment env;

    define_native_function(env, symbol("+"), add_integers);
    define_native_function(env, symbol("-"), subtract_integers);
    define_native_function(env, symbol("="), is_equal);
    define_native_function(env, symbol("not="), is_unequal);
    define_native_function(env, symbol("not"), not_f);

    return env;
}

}
