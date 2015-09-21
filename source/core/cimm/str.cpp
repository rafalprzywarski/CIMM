#include "str.hpp"
#include <sstream>

namespace cimm
{

namespace
{

template <typename C>
string join(char open, C const& c, char close)
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
        string operator()(const native_function& f) const { return "function"; }
    };

    return apply(to_string(), e);
}

}
