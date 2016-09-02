#pragma once
#include "expression.hpp"
#include "list.hpp"
#include "type_error.hpp"
#include "persistent_vector.hpp"

namespace cimm
{

class vector
{
public:
    vector() = default;
    explicit vector(list l)
    {
        for (; not is_empty(l); l = rest(l))
            value = value.push_back(first(l));
    }

    template <typename Iterator>
    vector(Iterator first, Iterator last) : value(first, last) { }

    vector(const std::initializer_list<expression>& l) : value(l) { }

    explicit vector(const std::vector<expression>& v) : vector(begin(v), end(v)) { }

    vector(persistent_vector<expression> v) : value(std::move(v)) { }

    friend auto begin(const vector& v)
    {
        return v.value.begin();
    }

    friend auto end(const vector& v)
    {
        return v.value.end();
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
        persistent_vector<expression> r;
        for (auto& e : v)
          r = r.push_back(f(e));
        return vector(r);
    }

    friend auto conj(const vector& v, expression e)
    {
        return vector(v.value.push_back(e));
    }

private:
    persistent_vector<expression> value;
};

inline auto is_empty(const vector& v)
{
    return begin(v) == end(v);
}

inline auto first(const vector& v)
{
    return is_empty(v) ? nil : *begin(v);
}

inline auto rest(const vector& l)
{
    return is_empty(l) ? vector{} : vector(std::next(begin(l)), end(l));
}

template <typename expression_type>
inline auto conj(const expression_type& e, const expression& ) -> expression
{
    throw type_error(e, "a sequence");
}

inline auto conj(const expression& seq, const expression& e) -> expression
{
    return apply([&](auto& seq) -> expression { return conj(seq, e); }, seq);
}

}
