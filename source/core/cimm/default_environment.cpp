#include "default_environment.hpp"
#include "error.hpp"

namespace cimm
{

namespace
{

auto add_integers(const list& l) -> expression
{
    integer sum{0};
    for (auto e = begin(l); e != end(l); ++e)
        sum += as_integer(*e);
    return sum;
}

auto subtract_integers(const list& l) -> expression
{
    return as_integer(first(l)) - as_integer(first(rest(l)));
}

auto is_equal(const list& l) -> boolean
{
    auto e = begin(l);
    if (e == end(l))
      throw arity_error(count(l), "=");
    auto first = *e;
    while (++e != end(l))
        if (first != *e)
            return false;
    return true;
}

auto is_equal_f(const list& l) -> expression
{
    return is_equal(l);
}

auto is_unequal_f(const list& l) -> expression
{
    return not is_equal(l);
}

struct visit_not : expression::visitor<boolean>
{
    boolean operator()(const boolean& b) const { return not b; }
    boolean operator()(const nil_type& ) const { return true; }

    template <typename T>
    boolean operator()(const T& ) const { return false; }
};

auto not_f(const list& args) -> expression
{
    return is_empty(args) || apply(visit_not(), first(args));
}

struct make_keyword : expression::visitor<expression>
{
    expression operator()(const string& s) const { return keyword(s); }
    expression operator()(const keyword& k) const { return k; }

    template <typename T>
    expression operator()(const T& ) const { return nil; }
};

auto keyword_f(const list& l) -> expression
{
  return is_empty(l) ? nil : apply(make_keyword(), first(l));
}

struct make_symbol : expression::visitor<expression>
{
    expression operator()(const string& s) const { return symbol(s); }

    template <typename T>
    expression operator()(const T& e) const { throw cannot_create_symbol(e); }
};

auto symbol_f(const list& l) -> expression
{
    if (is_empty(l))
        throw cannot_create_symbol(nil);
    return apply(make_symbol(), first(l));
}

auto list_f(const list& l) -> expression
{
    return l;
}

auto first_f(const list& l) -> expression
{
    if (count(l) != 1)
        throw arity_error(count(l), "first");
    list a = as_list(first(l));
    return is_empty(a) ? nil : first(a);
}

auto rest_f(const list& args) -> expression
{
    if (is_empty(args))
        return list{};
    auto f = first(args);
    if (f == nil)
        return list{};
    return rest(as_list(f));
}

auto count_f(const list& args) -> expression
{
    if (is_empty(args))
        return integer(0);
    auto f = first(args);
    if (f == nil)
        return integer(0);
    return count(as_list(f));
}

auto cons_f(const list& args) -> expression
{
    if (is_empty(args))
        return list{nil};
    auto elem = first(args);
    auto r = rest(args);
    if (is_empty(r))
        return list{elem};
    return cons(elem, as_list(first(r)));
}

auto conj_f(const list& args) -> expression
{
    return conj(first(args), first(rest(args)));
}

auto vector_f(const list& args) -> expression
{
    return vector(args);
}

auto vec_f(const list& args) -> expression
{
    return vector(as_list(first(args)));
}

}

auto create_default_environment() -> environment
{
    environment env;

    define_native_function(env, symbol("+"), add_integers);
    define_native_function(env, symbol("-"), subtract_integers);
    define_native_function(env, symbol("="), is_equal_f);
    define_native_function(env, symbol("not="), is_unequal_f);
    define_native_function(env, symbol("not"), not_f);
    define_native_function(env, symbol("keyword"), keyword_f);
    define_native_function(env, symbol("symbol"), symbol_f);
    define_native_function(env, symbol("list"), list_f);
    define_native_function(env, symbol("first"), first_f);
    define_native_function(env, symbol("rest"), rest_f);
    define_native_function(env, symbol("count"), count_f);
    define_native_function(env, symbol("cons"), cons_f);
    define_native_function(env, symbol("conj"), conj_f);
    define_native_function(env, symbol("vector"), vector_f);
    define_native_function(env, symbol("vec"), vec_f);

    return env;
}

}
