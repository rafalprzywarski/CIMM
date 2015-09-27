#pragma once
#include "expression.hpp"

namespace cimm
{

class slist
{
public:
    integer count() const { return 0; }
    expression first() const { return nil; }
};

}
