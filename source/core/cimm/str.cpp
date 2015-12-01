#include "str.hpp"
#include "expression.hpp"
#include <sstream>

namespace cimm
{

auto pr_str(vector const& v) -> string
{
    std::ostringstream os;
    os << '[';
    for (auto e = begin(v); e != end(v); ++e)
    {
        if (e != begin(v))
          os << ' ';
        os << pr_str(*e);
    }
    os << ']';
    return os.str();
}

auto pr_str(list const& l) -> string
{
    std::ostringstream os;
    os << '(';
    for (auto e = l; not is_empty(e); e = rest(e))
    {
        if (count(e) != count(l))
          os << ' ';
        os << pr_str(first(e));
    }
    os << ')';
    return os.str();
}

namespace
{

struct to_string : expression::visitor<string>
{
    template <typename expression_type>
    string operator()(const expression_type& e) const { return pr_str(e); }
    string operator()(const string& s) const { return s; }
};

}

auto str(const expression& e) -> string
{
    return apply(to_string(), e);
}

auto pr_str(const expression& e) -> string
{
    return apply([](auto& e) { return pr_str(e); }, e);
}

}
