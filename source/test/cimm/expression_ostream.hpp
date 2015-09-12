#pragma once
#include <ostream>
#include <cimm/str.hpp>

namespace cimm
{

inline auto operator<<(std::ostream& os, const expression& e) -> std::ostream&
{
    return os << str(e);
}

}
