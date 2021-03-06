#include "eval.hpp"
#include "error.hpp"
#include "type_error.hpp"

namespace cimm
{

namespace
{

auto evaluate_quote(const list& args)
{
    return first(args);
}

auto evaluate_def(environment& env, const list& args) -> expression
{
    auto s = first(args);
    if (env.definitions.count(str(s)) != 0)
        throw symbol_already_defined(as_symbol(s));
    env.definitions.emplace(str(s), evaluate_expression(env, first(rest(args))));
    return nil;
}

struct fn_visitor : expression::visitor<function>
{
    const list& args;
    fn_visitor(const list& args) : args(args) { }

    auto operator()(const vector& params) const -> function
    {
        return function{{{params, rest(args)}}};
    }

    auto operator()(const list& ) const -> function
    {
        auto defs = args;
        function f;
        f.overloads.reserve(count(defs));
        for (; !is_empty(defs); defs = rest(defs))
        {
            auto def = as_list(first(defs));
            auto params = as_vector(first(def));
            auto body = rest(def);
            f.overloads.push_back({params, body});
        }
        return f;
    }

    template <typename other>
    auto operator()(const other& e) const -> function
    {
        throw type_error(e, "a vector");
    }
};

auto evaluate_fn(environment& env, const list& args) -> function
{
    if (is_empty(args))
        return function{};
    return apply(fn_visitor{args}, first(args));
}

auto evaluate_if(environment& env, const list& args) -> expression
{
    if (count(args) < 2 || count(args) > 3)
        throw arity_error(count(args), "if");
    auto cond = evaluate_expression(env, first(args));
    auto is_true = cond != false && cond != nil;
    return evaluate_expression(env, first(rest(is_true ? args : rest(args))));
}

struct visit_catch : expression::visitor<expression>
{
    environment& env;
    expression handler;
    visit_catch(environment& env, const expression& handler) : env(env), handler(handler) { }

    auto operator()(const error& e) { return evaluate_expression(env, list{handler, unwrap(e)}); }

    template <typename expression_type>
    auto operator()(const expression_type& e) { return e; }
};

auto evaluate_catch(environment& env, const list& args) -> expression
{
    return apply(visit_catch{env, first(rest(args))}, evaluate_expression(env, first(args)));
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
        return list{special::quote, first(values)};
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

auto replace_symbols_fn(const list& seq, const vector& symbols, const list& values) -> list
{
    auto params = first(rest(seq));
    return list{special::fn, params, replace_symbols(first(rest(rest(seq))), remove_params(symbols, as_vector(params)), values)};
}

auto get_bindings_symbols(const vector& bindings)
{
    std::vector<expression> symbols;
    symbols.reserve(count(bindings) / 2);
    for (auto it = begin(bindings); it != end(bindings); it += 2)
        symbols.push_back(it[0]);
    return vector{symbols};
}

auto get_bindings_expressions(const vector& bindings)
{
    std::vector<expression> expressions;
    expressions.reserve(count(bindings) / 2);
    for (auto it = begin(bindings); it != end(bindings); it += 2)
        expressions.push_back(it[1]);
    return list{expressions};
}

auto replace_symbols_bindings(const vector& bindings, const vector& symbols, const list& values) -> vector
{
    std::vector<expression> replaced;
    replaced.reserve(count(bindings));
    for (auto it = begin(bindings); it != end(bindings); it += 2)
    {
        replaced.push_back(it[0]);
        replaced.push_back(replace_symbols(it[1], symbols, values));
    }
    return vector{replaced};
}

auto replace_symbols_let(const list& seq, const vector& symbols, const list& values) -> list
{
    auto bindings = replace_symbols_bindings(as_vector(first(rest(seq))), symbols, values);
    auto body = first(rest(rest(seq)));

    return list{special::let, bindings, replace_symbols(body, remove_params(symbols, get_bindings_symbols(bindings)), values)};
}

auto replace_symbols(const list& seq, const vector& symbols, const list& values) -> list
{
    if (first(seq) == special::fn)
        return replace_symbols_fn(seq, symbols, values);
    if (first(seq) == special::let)
        return replace_symbols_let(seq, symbols, values);
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
    auto body = overload.body;
    expression result;
    for (; !is_empty(body); body = rest(body))
        result = evaluate_expression(env, apply([&](auto& e) -> expression { return replace_symbols(e, overload.params, args); }, first(body)));
    return result;
}

auto execute(environment& env, const function& f, const list& args) -> expression
{
    auto overload = std::find_if(begin(f.overloads), end(f.overloads), [&](auto& o) { return count(o.params) == count(args); });
    if (overload == end(f.overloads))
        throw arity_error(count(args), "fn");
    return execute(env, *overload, args);
}

auto execute(environment& env, const generic_method& e, const list& args) -> expression
{
    auto methods = get_concrete_methods(e, args);
    if (methods.empty())
        throw no_matching_method_found_error(name(e));
    return execute(env, methods.front(), args);
}

template <typename expression_type>
auto execute(environment&, const expression_type& e, const list&) -> expression
{
    throw call_error(e);
}

auto find_error(const list& l) -> expression
{
    for (auto it = l; not is_empty(it); it = rest(it))
        if (is_error(first(it)))
            return first(it);
    return nil;
}

auto evaluate_call(environment& env, const list& l) -> expression
{
    auto evaluated = map(l, [&env](auto const& a) { return evaluate_expression(env, a); });
    auto error = find_error(evaluated);
    if (error != nil)
        return error;
    return apply([&](const auto& first) { return execute(env, first, rest(evaluated)); }, first(evaluated));
}

auto evaluate_let(environment& env, const list& l) -> expression
{
    auto bindings = as_vector(first(l));
    if (count(bindings) % 2 != 0)
        throw let_forms_error();

    auto evaluated = map(get_bindings_expressions(bindings), [&env](auto const& e) { return evaluate_expression(env, e); });
    return evaluate_expression(env, replace_symbols(first(rest(l)), get_bindings_symbols(bindings), evaluated));
}

auto evaluate_defgeneric(environment& env, const list& l) -> expression
{
    auto name = as_symbol(first(l));
    env.definitions.insert({str(name), generic_method(name)});
    return nil;
}

auto evaluate_defmethod(environment& env, const list& l) -> expression
{
    auto name = as_symbol(first(l));
    auto& m = as_generic_method(env.definitions.find(str(name))->second);
    define_method(m, evaluate_fn(env, rest(l)));
    return nil;
}

auto evaluate(environment& env, const list& l) -> expression
{
    auto name = first(l);
    if (name == special::quote)
        return evaluate_quote(rest(l));
    if (name == special::def)
        return evaluate_def(env, rest(l));
    if (name == special::fn)
        return evaluate_fn(env, rest(l));
    if (name == special::if_)
        return evaluate_if(env, rest(l));
    if (name == special::catch_)
        return evaluate_catch(env, rest(l));
    if (name == special::let)
        return evaluate_let(env, rest(l));
    if (name == special::defgeneric)
        return evaluate_defgeneric(env, rest(l));
    if (name == special::defmethod)
        return evaluate_defmethod(env, rest(l));
    return evaluate_call(env, l);
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
