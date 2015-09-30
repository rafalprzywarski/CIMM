#pragma once
#include "expression.hpp"
#include <vector>

namespace cimm
{

class list
{
public:
    list() = default;
    list(const std::initializer_list<expression>& l) : value(l) { }
    list(const std::vector<expression>& v) : value(v) { }
private:
    std::vector<expression> value;

    friend auto count(const list& l) -> integer
    {
        return l.value.size();
    }

    friend auto is_empty(const list& l)
    {
        return l.value.empty();
    }

    template <typename F>
    friend auto map(list const& l, F&& f)
    {
        std::vector<expression> r;
        r.reserve(count(l));
        for (auto& e : l.value)
          r.push_back(f(e));
        return list(r);
    }

    friend auto first(const list& l)
    {
        return is_empty(l) ? nil : *begin(l.value);
    }

    friend auto rest(const list& l)
    {
        return is_empty(l) ? list{} : list({std::next(begin(l.value)), end(l.value)});
    }

    friend auto cons(expression e, const list& l)
    {
        std::vector<expression> v;
        v.reserve(count(l) + 1);
        v.push_back(std::move(e));
        v.insert(end(v), begin(l.value), end(l.value));
        return list(v);
    }

    friend auto operator==(const list& left, const list& right)
    {
        return left.value == right.value;
    }
};

inline auto conj(const list& l, expression e)
{
    return cons(e, l);
}

}
