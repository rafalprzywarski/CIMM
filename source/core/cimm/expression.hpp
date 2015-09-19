#pragma once
#include <string>
#include <vector>
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

struct symbol
{
    string value;

    symbol() = default;
    explicit symbol(string value) : value(value) { }
};

static const symbol quote{"quote"};

inline auto operator==(const symbol& left, const symbol& right)
{
    return left.value == right.value;
}

struct keyword
{
    string value;

    keyword() = default;
    explicit keyword(string value) : value(std::move(value)) { }
};

inline auto operator==(const keyword& left, const keyword& right)
{
    return left.value == right.value;
}

struct list;
using expression_variant = boost::variant<nil_type, symbol, keyword, string, integer, boolean, boost::recursive_wrapper<list>>;

struct expression
{
    expression_variant value;

    expression() = default;
    expression(const nil_type& n) : value(n) { }
    expression(const symbol& s) : value(s) { }
    expression(const keyword& k) : value(k) { }
    expression(const string& s) : value(s) { }
    expression(const integer& i) : value(i) { }
    expression(const boolean& b) : value(b) { }
    expression(const list& l) : value(l) { }
    expression(const expression_variant& v) : value(v) { }
};

inline auto operator==(const expression& left, const expression& right)
{
    return left.value == right.value;
}

inline auto operator!=(const expression& left, const expression& right)
{
    return !(left == right);
}

struct list
{
    std::vector<expression> value;

    list() = default;
    list(const std::initializer_list<expression>& l) : value(l) { }
    list(const std::vector<expression>& v) : value(v) { }
};

inline auto is_empty(const list& l)
{
    return l.value.empty();
}

inline auto first(const list& l)
{
    return l.value.front();
}

inline auto rest(const list& l)
{
    return is_empty(l) ? list{} : list({std::next(std::begin(l.value)), std::end(l.value)});
}

inline auto count(const list& l) -> integer
{
    return l.value.size();
}

template <typename F>
inline auto map(list l, F&& f)
{
    std::vector<expression> r;
    r.reserve(l.value.size());
    for (auto& e : l.value)
      r.push_back(f(e));
    return r;
}

inline auto operator==(const list& left, const list& right)
{
    return left.value == right.value;
}

template <typename result_type>
struct visitor : boost::static_visitor<result_type> { };

template <typename Visitor>
inline auto apply(Visitor&& v, expression const& e)
{
    return boost::apply_visitor(v, e.value);
}

}
