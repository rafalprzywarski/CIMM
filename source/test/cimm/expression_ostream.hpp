#pragma once
#include <ostream>
#include <cimm/str.hpp>

namespace cimm
{

auto operator<<(std::ostream& os, const expression& e) -> std::ostream&
{
    return os << str(e);
}

}
