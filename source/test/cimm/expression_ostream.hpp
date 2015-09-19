#pragma once
#include <ostream>
#include <cimm/str.hpp>

namespace cimm
{

inline auto operator<<(std::ostream& os, const nil_type& e) -> std::ostream&
{
    return os << str(e);
}

inline auto operator<<(std::ostream& os, const expression& e) -> std::ostream&
{
    return os << str(e);
}

}
