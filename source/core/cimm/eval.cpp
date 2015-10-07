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

struct fn_visitor : expression::visitor<expression>
{
    const list& args;
    fn_visitor(const list& args) : args(args) { }

    auto operator()(const vector& params) const -> expression
    {
        return function{{{params, first(rest(args))}}};
    }

    auto operator()(const list& ) const -> expression
    {
        auto defs = args;
        function f;
        f.overloads.reserve(count(defs));
        for (; !is_empty(defs); defs = rest(defs))
        {
            auto def = as_list(first(defs));
            auto params = as_vector(first(def));
            auto body = first(rest(def));
            f.overloads.push_back({params, body});
        }
        return f;
    }

    template <typename other>
    auto operator()(const other& ) const -> expression
    {
        return nil;
    }
};

auto fn(environment& env, const list& args) -> expression
{
    return apply(fn_visitor{args}, first(args));
}

auto if_(environment& env, const list& args) -> expression
{
    if (count(args) < 2)
        throw arity_error("too few arguments to if");
    if (count(args) > 3)
        throw arity_error("too many arguments to if");
    auto cond = evaluate_expression(env, first(args));
    auto is_true = cond != false && cond != nil;
    return evaluate_expression(env, first(rest(is_true ? args : rest(args))));
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

    return map(seq, [&](auto const& e) { return replace_symbols(e, symbols, values); });
}

auto replace_symbols(const expression& e, const vector& symbols, const list& values) -> expression
{
    return apply([&](auto& e) -> expression { return replace_symbols(e, symbols, values); }, e);
}

auto execute(environment& env, const function::overload& overload, const list& args) -> expression
{
    if (is_empty(overload.params))
        return evaluate_expression(env, overload.body);
    return evaluate_expression(env, apply([&](auto& e) -> expression { return replace_symbols(e, overload.params, args); }, overload.body));
}

auto execute(environment& env, const function& f, const list& args) -> expression
{
    auto overload = std::find_if(begin(f.overloads), end(f.overloads), [&](auto& o) { return count(o.params) == count(args); });
    if (overload == end(f.overloads))
        return nil;
    return execute(env, *overload, args);
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
