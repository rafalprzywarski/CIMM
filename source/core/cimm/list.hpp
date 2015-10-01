#pragma once
#include "expression.hpp"
#include "slist.hpp"
#include <vector>
#include <boost/range/adaptor/reversed.hpp>

namespace cimm
{

class list
{
public:
    list() = default;
    list(const std::initializer_list<expression>& l)
    {
        for (auto& e : boost::adaptors::reverse(l))
            value = slist{e, value};
    }

    list(const std::vector<expression>& v)
    {
        for (auto& e : boost::adaptors::reverse(v))
            value = slist{e, value};
    }

private:
    slist value;

    list(slist l) : value(std::move(l)) { }

    friend auto count(const list& l) -> integer
    {
        return l.value.count();
    }

    friend auto is_empty(const list& l)
    {
        return l.value.count() == 0;
    }

    friend auto first(const list& l) -> expression
    {
        return l.value.first();
    }

    friend auto rest(const list& l)
    {
        return list(l.value.next());
    }

    friend auto cons(expression e, const list& l)
    {
        return list(l.value.cons(std::move(e)));
    }

    friend auto operator==(const list& left, const list& right)
    {
        return left.value == right.value;
    }
};

inline auto reverse(list l) -> list
{
    list r;
    for (; not is_empty(l); l = rest(l))
        r = cons(first(l), r);
    return r;
}

template <typename F>
inline auto map(list l, F&& f) -> list
{
    list mapped;
    for (; not is_empty(l); l = rest(l))
        mapped = cons(f(first(l)), mapped);
    return reverse(mapped);
}

inline auto conj(const list& l, expression e)
{
    return cons(e, l);
}

}
