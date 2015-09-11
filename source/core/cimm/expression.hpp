#pragma once
#include <string>
#include <vector>
#include <boost/variant.hpp>

namespace cimm
{

using string = std::string;
using integer = int;
using list = std::vector<integer>;
using expression_variant = boost::variant<string, integer, list>;

struct expression
{
    expression_variant value;

    template <typename type>
    expression(type&& v) : value(std::forward<type>(v)) { }
};

inline auto operator==(const expression& left, const expression& right)
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
