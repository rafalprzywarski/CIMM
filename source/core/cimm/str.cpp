#include "str.hpp"
#include "expression.hpp"
#include <sstream>

namespace cimm
{

namespace
{

string join(char open, vector const& c, char close)
{
    std::ostringstream os;
    os << open;
    for (auto e = begin(c); e != end(c); ++e)
    {
        if (e != begin(c))
          os << ' ';
        os << str(*e);
    }
    os << close;
    return os.str();
}

string join(char open, list const& l, char close)
{
    std::ostringstream os;
    os << open;
    for (auto e = l; not is_empty(e); e = rest(e))
    {
        if (count(e) != count(l))
          os << ' ';
        os << str(first(e));
    }
    os << close;
    return os.str();
}

}

auto str(const expression& e) -> string
{
    struct to_string : expression::visitor<string>
    {
        string operator()(const nil_type& ) const { return "nil"; }
        string operator()(const symbol& s) const { return str(s); }
        string operator()(const keyword& k) const { return str(k); }
        string operator()(const string& s) const { return s; }
        string operator()(const integer& v) const { return std::to_string(v); }
        string operator()(const boolean& b) const { return b ? "true" : "false"; }
        string operator()(const list& l) const { return join('(', l, ')'); }
        string operator()(const vector& v) const { return join('[', v, ']'); }
        string operator()(const native_function& ) const { return "fn"; }
        string operator()(const function& f) const { return "fn"; }
        string operator()(const error& e) const { return str(e); }
    };

    return apply(to_string(), e);
}

namespace
{

struct pr_to_string : expression::visitor<string>
{
    string operator()(const string& s) const { return '\"' + s + '\"'; }

    template <typename expression_type>
    string operator()(const expression_type& ) const { return {}; }
};

}

auto pr_str(const expression& e) -> string
{
    return apply(pr_to_string(), e);
}

}
