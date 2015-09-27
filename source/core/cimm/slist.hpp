#pragma once
#include "expression.hpp"

namespace cimm
{

class slist
{
public:
    slist() = default;
    slist(expression e, const slist& next)
        : count_(next.count() + 1), value(std::move(e)) { }
    integer count() const { return count_; }
    expression first() const { return value; }
    auto cons(expression e) const { return slist{e, *this}; }
    auto next() const { return slist{}; }
private:
    integer count_ = 0;
    expression value;
};

}
