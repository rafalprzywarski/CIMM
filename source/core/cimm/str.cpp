#include "str.hpp"
#include <sstream>

namespace cimm
{

auto str(const expression& e) -> string
{
    struct to_string : expression::visitor<string>
    {
        string operator()(const nil_type& ) const { return "nil"; }
        string operator()(const symbol& s) const { return str(s); }
        string operator()(const keyword& k) const { return ':' + str(k); }
        string operator()(const string& s) const { return s; }
        string operator()(const integer& v) const { return std::to_string(v); }
        string operator()(const boolean& b) const { return b ? "true" : "false"; }
        string operator()(const list& l) const
        {
            std::ostringstream os;
            os << '(';
            for (auto e = begin(l); e != end(l); ++e)
            {
                if (e != begin(l))
                  os << ' ';
                os << str(*e);
            }
            os << ')';
            return os.str();
        }
    };

    return apply(to_string(), e);
}

}
