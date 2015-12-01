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
        os << str(*e);
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
        os << str(first(e));
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

namespace
{

struct pr_to_string : expression::visitor<string>
{
    string operator()(const nil_type& nil) const { return pr_str(nil); }
    string operator()(const symbol& s) const { return pr_str(s); }
    string operator()(const keyword& k) const { return pr_str(k); }
    string operator()(const string& s) const { return pr_str(s); }
    string operator()(const integer& v) const { return pr_str(v); }
    string operator()(const boolean& b) const { return pr_str(b); }
    string operator()(const list& l) const { return pr_str(l); }
    string operator()(const vector& v) const { return pr_str(v); }
    string operator()(const native_function& f) const { return pr_str(f); }
    string operator()(const function& f) const { return pr_str(f); }
    string operator()(const error& e) const { return pr_str(e); }
};

}

auto pr_str(const expression& e) -> string
{
    return apply(pr_to_string(), e);
}

}
