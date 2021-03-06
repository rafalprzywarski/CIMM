#pragma once
#include "string.hpp"
#include "str.hpp"
#include <boost/variant.hpp>
#include <vector>

namespace cimm
{

using integer = int;

inline auto pr_str(const integer& v) -> string
{
    return std::to_string(v);
}

using boolean = bool;

inline auto pr_str(const boolean& b) -> string
{
    return b ? string{"true"} : string{"false"};
}

struct nil_type {};
static constexpr nil_type nil{};

inline auto operator==(const nil_type& left, const nil_type& right)
{
    return true;
}

inline auto pr_str(const nil_type& ) -> string
{
    return "nil";
}

class symbol
{
public:
    symbol() = default;
    explicit symbol(string value) : value(value) { }

    friend auto operator==(const symbol& left, const symbol& right)
    {
        return left.value == right.value;
    }

    friend auto pr_str(const symbol& s) -> string const&
    {
        return s.value;
    }

private:
    string value;
};

namespace special
{

static const symbol quote{"quote"};
static const symbol def{"def"};
static const symbol fn{"fn"};
static const symbol if_{"if"};
static const symbol catch_{"catch"};
static const symbol let{"let"};
static const symbol defgeneric{"defgeneric"};
static const symbol defmethod{"defmethod"};

}

class keyword
{
public:
    keyword() = default;
    explicit keyword(string value) : value(std::move(value)) { }

    friend auto operator==(const keyword& left, const keyword& right)
    {
        return left.value == right.value;
    }

    friend auto pr_str(const keyword& k) -> string
    {
        return ':' + k.value;
    }

private:
    string value;
};

class list;
class vector;
struct function;
class expression;
class error;
struct environment;

using native_function_va = expression(*)(list const&);
using native_function_0 = expression(*)();
using native_function_1 = expression(*)(const expression&);
using native_function_2 = expression(*)(const expression&, const expression&);

class native_function
{
public:
    template <typename func>
    native_function(const string& name, func f) : name(name), f(f) { }

    auto operator()(const list& args) const -> expression;

    friend auto name(const native_function& f) { return f.name; }

    friend auto operator==(const native_function& left, const native_function& right)
    {
        return left.f == right.f;
    }

private:

    auto is_va() const { return f.which() == 0; }

    auto has_args(integer n) const { return f.which() == (n + 1); }

    auto verify_accepts_n_args(integer n) const -> void;

    using variant = boost::variant<
        native_function_va,
        native_function_0,
        native_function_1,
        native_function_2
    >;

    string name;
    variant f;
};

inline auto pr_str(const native_function& ) -> string
{
    return string{"fn"};
}

template <typename result_type>
struct native_function_visitor : boost::static_visitor<result_type> { };

class generic_method;

using expression_variant = boost::variant<
    nil_type,
    symbol,
    keyword,
    string,
    integer,
    boolean,
    boost::recursive_wrapper<list>,
    boost::recursive_wrapper<vector>,
    native_function,
    boost::recursive_wrapper<function>,
    boost::recursive_wrapper<error>,
    boost::recursive_wrapper<generic_method>
>;

class expression
{
public:
    expression() = default;
    expression(const nil_type& n) : value(n) { }
    expression(const symbol& s) : value(s) { }
    expression(const keyword& k) : value(k) { }
    expression(const string& s) : value(s) { }
    expression(const integer& i) : value(i) { }
    expression(const boolean& b) : value(b) { }
    expression(const list& l) : value(l) { }
    expression(const vector& v) : value(v) { }
    expression(native_function f) : value(f) { }
    expression(const function& f) : value(f) { }
    expression(const expression_variant& v) : value(v) { }
    expression(const error& e) : value(e) { }
    expression(const generic_method& m) : value(m) { }

    template <typename result_type>
    struct visitor : boost::static_visitor<result_type> { };

    template <typename Visitor>
    friend auto apply(Visitor&& v, expression const& e)
    {
        return boost::apply_visitor(v, e.value);
    }

    friend auto as_list(const expression& e) -> list const&;
    friend auto as_vector(const expression& e) -> vector const&;
    friend auto as_symbol(const expression& e) -> symbol const&;
    friend auto as_integer(const expression& e) -> integer;
    friend auto is_error(const expression& e) -> bool;
    friend auto as_generic_method(expression& e) -> generic_method&;

private:
    expression_variant value;

    friend auto operator==(const expression& left, const expression& right)
    {
        return left.value == right.value;
    }
};

inline auto operator!=(const expression& left, const expression& right)
{
    return !(left == right);
}

inline auto operator==(const function&, const function&)
{
    return false;
}

class error
{
public:
    explicit error(const expression& e) : value(e) { }

    friend auto operator==(const error& left, const error& right)
    {
        return left.value == right.value;
    }

    friend auto pr_str(const error& e) -> string
    {
        return string("error: ") + cimm::pr_str(e.value);
    }

    friend auto unwrap(const error& e)
    {
        return e.value;
    }

private:
    expression value;
};

}

#include "list.hpp"
#include "vector.hpp"
#include "error.hpp"

namespace cimm
{

struct function
{
    struct overload
    {
        vector params;
        list body;
    };
    std::vector<overload> overloads;
};

inline auto pr_str(const function& ) -> string
{
    return string{"fn"};
}

inline auto native_function::verify_accepts_n_args(integer n) const -> void
{
    if (!is_va() && !has_args(n))
        throw arity_error(n, name);
}

inline auto native_function::operator()(const list& args) const -> expression
{
    verify_accepts_n_args(count(args));

    struct evaluate : boost::static_visitor<expression>
    {
        const list& args;
        evaluate(const list& args) : args(args) { }

        auto operator()(const native_function_va& f) { return f(args); }
        auto operator()(const native_function_0& f) { return f(); }
        auto operator()(const native_function_1& f) { return f(first(args)); }
        auto operator()(const native_function_2& f) { return f(first(args), first(rest(args))); }
    };

    evaluate v(args);
    return boost::apply_visitor(v, f);
}

class generic_method
{
public:
    generic_method(symbol name) : name(name) { }

    friend auto name(const generic_method& m) { return m.name; }

    friend auto operator==(const generic_method& left, const generic_method& right)
    {
        return left.name == right.name;
    }

    friend auto pr_str(const generic_method& m) -> string
    {
        return "generic method " + pr_str(m.name);
    }

    friend auto get_concrete_methods(const generic_method& m, const list& ) -> std::vector<function>
    {
        return m.methods;
    }

    friend auto define_method(generic_method& g, function m)
    {
        g.methods.push_back(m);
    }

private:
    symbol name;
    std::vector<function> methods;
};

}
