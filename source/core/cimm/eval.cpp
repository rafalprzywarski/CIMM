#include "eval.hpp"
#include "error.hpp"

namespace cimm
{

namespace
{

auto def(environment& env, const list& args) -> expression
{
    auto s = first(args);
    if (env.definitions.count(str(s)) != 0)
        throw symbol_already_defined(as_symbol(s));
    env.definitions[str(s)] = first(rest(args));
    return nil;
}

auto evaluate(environment& env, const list& l) -> expression
{
    auto name = as_symbol(first(l));
    if (name == special::quote)
        return first(rest(l));
    if (name == special::def)
        return def(env, rest(l));
    auto f = env.functions.find(str(name));
    if (f == env.functions.end())
        throw undefined_function_error(name);
    return f->second(env, map(rest(l), [&env](auto const& a) { return evaluate_expression(env, a); }));
}

auto evaluate(environment& env, const symbol& s) -> expression
{
    return env.definitions[str(s)];
}

template <typename expression_type>
auto evaluate(environment&, const expression_type& e) -> expression
{
    return e;
}

}

auto evaluate_expression(environment& env, const expression& expr) -> expression
{
    return apply([&env] (auto& e) { return evaluate(env, e); }, expr);
}

}
