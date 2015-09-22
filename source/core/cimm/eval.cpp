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
    env.definitions.emplace(str(s), evaluate_expression(env, first(rest(args))));
    return nil;
}

auto fn(environment& env, const list& args) -> expression
{
    return function{as_vector(first(args)), first(rest(args))};
}

auto execute(environment& env, native_function f, const list& args) -> expression
{
    return f(env, args);
}

auto execute(environment& env, const function& f, const list& args) -> expression
{
    if (is_empty(f.params))
        return evaluate_expression(env, f.body);
    auto body = as_list(f.body);
    return evaluate_expression(env, cons(first(body), cons(first(args), rest(rest(body)))));
}

template <typename expression_type>
auto execute(environment& env, const expression_type& first, const list& args) -> expression
{
    return nil;
}

auto evaluate(environment& env, const list& l) -> expression
{
    auto name = first(l);
    if (name == special::quote)
        return first(rest(l));
    if (name == special::def)
        return def(env, rest(l));
    if (name == special::fn)
        return fn(env, rest(l));
    auto evaluated = map(l, [&env](auto const& a) { return evaluate_expression(env, a); });
    return apply([&](const auto& first) { return execute(env, first, rest(evaluated)); }, first(evaluated));
}

auto evaluate(environment& env, const symbol& s) -> expression
{
    auto found = env.definitions.find(str(s));
    if (found == env.definitions.end())
        throw undefined_symbol_error(s);
    return found->second;
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
