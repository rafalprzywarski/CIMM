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

    friend auto begin(const list& l)
    {
        return begin(l.value);
    }

    friend auto end(const list& l)
    {
        return end(l.value);
    }

    friend auto count(const list& l) -> integer
    {
        return l.value.size();
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

    friend auto operator==(const list& left, const list& right)
    {
        return left.value == right.value;
    }
};

inline auto is_empty(const list& l)
{
    return begin(l) == end(l);
}

inline auto first(const list& l)
{
    return is_empty(l) ? nil : *begin(l);
}

inline auto rest(const list& l)
{
    return is_empty(l) ? list{} : list({std::next(begin(l)), end(l)});
}

inline auto cons(expression e, const list& l)
{
    std::vector<expression> v;
    v.reserve(count(l) + 1);
    v.push_back(std::move(e));
    v.insert(end(v), begin(l), end(l));
    return list(v);
}

inline auto conj(const list& l, expression e)
{
    return cons(e, l);
}

}
