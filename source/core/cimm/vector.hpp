#pragma once
#include "expression.hpp"
#include "list.hpp"
#include <vector>

namespace cimm
{

class vector
{
public:
    vector() = default;
    explicit vector(const list& l) : value(begin(l), end(l)) { }
    vector(const std::initializer_list<expression>& l) : value(l) { }
    explicit vector(const std::vector<expression>& v) : value(std::move(v)) { }

    friend auto begin(const vector& v)
    {
        return begin(v.value);
    }

    friend auto end(const vector& v)
    {
        return end(v.value);
    }

    friend auto count(const vector& v) -> integer
    {
        return v.value.size();
    }

    friend auto operator==(const vector& left, const vector& right)
    {
        return left.value == right.value;
    }

    template <typename F>
    friend auto map(vector const& v, F&& f)
    {
        std::vector<expression> r;
        r.reserve(count(v));
        for (auto& e : v)
          r.push_back(f(e));
        return vector(r);
    }
private:
    std::vector<expression> value;
};

inline auto is_empty(const vector& v)
{
    return begin(v) == end(v);
}

inline auto first(const vector& v)
{
    return *begin(v);
}

inline auto rest(const vector& l)
{
    return is_empty(l) ? vector{} : vector({std::next(begin(l)), end(l)});
}

struct function
{
    vector params;
    expression body;
};

inline auto operator==(const function&, const function&)
{
    return false;
}

inline auto conj(const vector& v, expression e)
{
    std::vector<expression> r;
    r.reserve(count(v) + 1);
    r.assign(begin(v), end(v));
    r.push_back(std::move(e));
    return vector(r);
}

template <typename expression_type>
inline auto conj(const expression_type& , const expression& )
{
    return nil;
}

inline auto conj(const expression& seq, const expression& e) -> expression
{
    return apply([&](auto& seq) -> expression { return conj(seq, e); }, seq);
}

}
