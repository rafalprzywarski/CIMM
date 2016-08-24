#include "default_environment.hpp"
#include "error.hpp"
#include "type_cast.hpp"
#include <iostream>

namespace cimm
{

namespace
{

auto add_integers(const list& l) -> expression
{
    integer sum{0};
    for (auto e = l; not is_empty(e); e = rest(e))
        sum += as_integer(first(e));
    return sum;
}

auto multiply_integers(const list& l) -> expression
{
    integer prod{1};
    for (auto e = l; not is_empty(e); e = rest(e))
        prod *= as_integer(first(e));
    return prod;
}

auto subtract_integers(const list& l) -> expression
{
    if (is_empty(l))
        throw arity_error(count(l), "-");
    if (count(l) == 1)
        return -as_integer(first(l));
    integer d = as_integer(first(l));
    for (auto e = rest(l); not is_empty(e); e = rest(e))
        d -= as_integer(first(e));
    return d;
}

auto is_equal(const list& l) -> boolean
{
    if (is_empty(l))
      throw arity_error(count(l), "=");
    auto c = first(l);
    for (auto e = rest(l); not is_empty(e); e = rest(e))
        if (c != first(e))
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

auto is_less_than_f(const list& l) -> expression
{
    auto x = as_integer(first(l));
    auto it = rest(l);
    while (not is_empty(it))
    {
        auto y = as_integer(first(it));
        if (not (x < y))
            return false;
        x = y;
        it = rest(it);
    }
    return true;
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
    if (is_empty(l))
        throw arity_error(count(l), "keyword");
    return apply(make_keyword(), first(l));
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
        throw arity_error(count(l), "symbol");
    return apply(make_symbol(), first(l));
}

auto list_f(const list& l) -> expression
{
    return l;
}

namespace
{

struct first_visitor : expression::visitor<expression>
{
    auto operator()(const list& l) -> expression { return first(l); }
    auto operator()(const vector& v) -> expression { return first(v); }

    template <typename expression_type>
    auto operator()(const expression_type& e) -> expression { throw type_error(e, "a sequence"); }
};

}

auto first_f(const expression& e) -> expression
{
    return apply(first_visitor(), e);
}

auto rest_f(const expression& e) -> expression
{
    if (e == nil)
        return list{};
    return rest(as_list(e));
}

namespace
{

struct count_visitor : expression::visitor<expression>
{
    auto operator()(const list& l) -> expression { return count(l); }
    auto operator()(const vector& v) -> expression { return count(v); }
    auto operator()(const nil_type&) -> expression { return integer(0); }

    template <typename expression_type>
    auto operator()(const expression_type& e) -> expression { throw type_error(e, "a sequence"); }
};

}

auto count_f(const expression& c) -> expression
{
    return apply(count_visitor(), c);
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

auto nth_f(const list& args) -> expression
{
    return *(begin(as_vector(first(args))) + as_integer(first(rest(args))));
}

auto vector_f(const list& args) -> expression
{
    return vector(args);
}

auto vec_f(const list& args) -> expression
{
    return vector(as_list(first(args)));
}

auto throw_f(const expression& e) -> expression
{
    return error{e};
}

auto str_f(const list& args) -> expression
{
    std::string s;
    for (auto l = args; not is_empty(l); l = rest(l))
        s += str(first(l)).c_str();
    return string{s};
}

auto pr_str_f(const list& args) -> expression
{
    std::string s;
    for (auto l = args; not is_empty(l); l = rest(l))
        s += pr_str(first(l)).c_str();
    return string{s};
}

auto print_f(const list& args) -> expression
{
    if (is_empty(args))
        return nil;
    std::cout << str(first(args));
    for (auto l = rest(args); not is_empty(l); l = rest(l))
        std::cout << ' ' << str(first(l));
    std::cout << std::flush;
    return nil;
}

auto exit_f(const expression& exit_code) -> expression
{
    std::exit(as_integer(exit_code));
}

}

auto create_default_environment() -> environment
{
    environment env;

    define_native_function(env, {"+", add_integers});
    define_native_function(env, {"*", multiply_integers});
    define_native_function(env, {"-", subtract_integers});
    define_native_function(env, {"=", is_equal_f});
    define_native_function(env, {"not=", is_unequal_f});
    define_native_function(env, {"<", is_less_than_f});
    define_native_function(env, {"not", not_f});
    define_native_function(env, {"keyword", keyword_f});
    define_native_function(env, {"symbol", symbol_f});
    define_native_function(env, {"list", list_f});
    define_native_function(env, {"first", first_f});
    define_native_function(env, {"rest", rest_f});
    define_native_function(env, {"count", count_f});
    define_native_function(env, {"cons", cons_f});
    define_native_function(env, {"conj", conj_f});
    define_native_function(env, {"nth", nth_f});
    define_native_function(env, {"vector", vector_f});
    define_native_function(env, {"vec", vec_f});
    define_native_function(env, {"throw", throw_f});
    define_native_function(env, {"str", str_f});
    define_native_function(env, {"pr-str", pr_str_f});
    define_native_function(env, {"print", print_f});
    define_native_function(env, {"exit", exit_f});

    return env;
}

}
