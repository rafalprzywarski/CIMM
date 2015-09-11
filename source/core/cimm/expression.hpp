#pragma once
#include <string>
#include <boost/variant.hpp>

namespace cimm
{

using string = std::string;
using integer = int;
using expression = boost::variant<string, integer>;

template <typename result_type>
struct visitor : boost::static_visitor<result_type> { };

template <typename Visitor>
inline auto apply(Visitor&& v, expression const& e)
{
    return boost::apply_visitor(v, e);
}

}
