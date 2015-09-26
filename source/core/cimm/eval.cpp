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

auto if_(environment& env, const list& args) -> expression
{
    auto cond = first(args);
    return evaluate_expression(env, first(rest(cond != false && cond != nil ? args : rest(args))));
}

auto execute(environment&, native_function f, const list& args) -> expression
{
    return f(args);
}

auto replace_symbols(const expression& e, const vector& symbols, const list& values) -> expression;

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

auto replace_symbols(const list& seq, const vector& symbols, const list& values) -> list;

auto replace_symbols(const vector& seq, const vector& symbols, const list& values) -> vector
{
    return map(seq, [&](auto& e) { return replace_symbols(e, symbols, values); });
}

auto remove_params(const vector& v, const vector& forbidden)
{
    return map(v, [&](auto& e) { return (std::find(begin(forbidden), end(forbidden), e) == end(forbidden)) ? e : nil; });
}

auto replace_symbols(const list& seq, const vector& symbols, const list& values) -> list
{
    if (first(seq) == special::fn)
    {
        auto params = first(rest(seq));
        return list{special::fn, params, replace_symbols(first(rest(rest(seq))), remove_params(symbols, as_vector(params)), values)};
    }
    if (first(seq) == special::quote)
        return seq;

    return map(seq, [&](auto& e) { return replace_symbols(e, symbols, values); });
}

auto replace_symbols(const expression& e, const vector& symbols, const list& values) -> expression
{
    return apply([&](auto& e) -> expression { return replace_symbols(e, symbols, values); }, e);
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
    if (name == special::if_)
        return if_(env, rest(l));
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

auto evaluate(environment& env, const vector& v) -> expression
{
    return map(v, [&](auto& e) { return evaluate_expression(env, e); });
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
