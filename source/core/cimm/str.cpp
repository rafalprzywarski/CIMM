#include "str.hpp"
#include <sstream>

namespace cimm
{

auto str(const expression& e) -> string
{
    struct to_string : visitor<string>
    {
        string operator()(const identifier& i) const { return i.value; }
        string operator()(const integer& v) const { return std::to_string(v); }
        string operator()(const boolean& b) const { return b ? "true" : "false"; }
        string operator()(const list& l) const
        {
            std::ostringstream os;
            os << '(';
            for (auto e = std::begin(l.value); e != std::end(l.value); ++e)
            {
                if (e != std::begin(l.value))
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
