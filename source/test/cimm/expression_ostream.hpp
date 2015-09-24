#pragma once
#include <ostream>
#include <cimm/str.hpp>

namespace cimm
{

inline auto operator<<(std::ostream& os, const nil_type& e) -> std::ostream&
{
    return os << str(e);
}

inline auto operator<<(std::ostream& os, const list& l) -> std::ostream&
{
    return os << str(l);
}

inline auto operator<<(std::ostream& os, const vector& v) -> std::ostream&
{
    return os << str(v);
}

inline auto operator<<(std::ostream& os, const symbol& s) -> std::ostream&
{
    return os << str(s);
}

inline auto operator<<(std::ostream& os, const expression& e) -> std::ostream&
{
    return os << str(e);
}

}
