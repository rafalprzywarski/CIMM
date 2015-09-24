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

auto execute(environment&, native_function f, const list& args) -> expression
{
    return f(args);
}

template <typename expression_type>
auto replace_symbols(const expression_type& e, const vector&, const list&) -> expression_type
{
    return e;
}

auto replace_symbols(const symbol& s, const vector& symbols, const list& values) -> expression
{
    if (is_empty(symbols))
        return s;
    if (s == first(symbols))
        return first(values);
    return replace_symbols(s, rest(symbols), rest(values));
}

auto replace_symbols(const list& l, const vector& symbols, const list& values) -> list
{
    if (is_empty(l))
        return l;
    return cons(
        apply([&](auto& e) -> expression { return replace_symbols(e, symbols, values); }, first(l)),
        replace_symbols(rest(l), symbols, values));
}

auto execute(environment& env, const function& f, const list& args) -> expression
{
    if (is_empty(f.params))
        return evaluate_expression(env, f.body);
    return evaluate_expression(env, apply([&](auto& e) -> expression { return replace_symbols(e, f.params, args); }, f.body));
}

template <typename expression_type>
auto execute(environment&, const expression_type&, const list&) -> expression
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
