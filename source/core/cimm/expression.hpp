#pragma once
#include "string.hpp"
#include <boost/variant.hpp>

namespace cimm
{

using string = std::string;
using integer = int;
using boolean = bool;
struct nil_type {};
static constexpr nil_type nil{};

inline auto operator==(const nil_type& left, const nil_type& right)
{
    return true;
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

    friend auto str(const symbol& s) -> string const&
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

    friend auto str(const keyword& k) -> string
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
struct environment;

using native_function_va = expression(*)(list const&);
using native_function_0 = expression(*)();
using native_function_1 = expression(*)(const expression&);
using native_function_2 = expression(*)(const expression&, const expression&);

using native_function = boost::variant<
    native_function_va,
    native_function_0,
    native_function_1,
    native_function_2
>;

template <typename result_type>
struct native_function_visitor : boost::static_visitor<result_type> { };

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
    boost::recursive_wrapper<function>
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

}

#include "list.hpp"
#include "vector.hpp"

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

}
