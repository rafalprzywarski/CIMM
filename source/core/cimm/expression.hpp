#pragma once
#include <string>
#include <vector>
#include <boost/variant.hpp>

namespace cimm
{

using string = std::string;
using integer = int;
using identifier = string;
struct list;
using expression_variant = boost::variant<identifier, integer, boost::recursive_wrapper<list>>;

struct expression
{
    expression_variant value;

    expression() = default;
    expression(const identifier& i) : value(i) { }
    expression(const integer& i) : value(i) { }
    expression(const list& l) : value(l) { }
    expression(const expression_variant& v) : value(v) { }
};

inline auto operator==(const expression& left, const expression& right)
{
    return left.value == right.value;
}

struct list
{
    std::vector<expression> value;

    list() = default;
    list(const std::initializer_list<expression>& l) : value(l) { }
    list(const std::vector<expression>& v) : value(v) { }
};

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
