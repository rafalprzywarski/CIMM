#pragma once
#include <string>
#include <vector>
#include <boost/variant.hpp>

namespace cimm
{

using string = std::string;
using integer = int;
using boolean = bool;
struct nil_type {};
static constexpr nil_type nil{};

inline auto operator==(const nil_type& left, const nil_type& right)
{
    return true;
}

class symbol
{
public:
    symbol() = default;
    explicit symbol(string value) : value(value) { }

    friend auto operator==(const symbol& left, const symbol& right)
    {
        return left.value == right.value;
    }

    friend auto str(const symbol& s) -> string const&
    {
        return s.value;
    }

private:
    string value;
};

namespace special
{

static const symbol quote{"quote"};
static const symbol def{"def"};
static const symbol fn{"fn"};
static const symbol if_{"if"};

}

class keyword
{
public:
    keyword() = default;
    explicit keyword(string value) : value(std::move(value)) { }

    friend auto operator==(const keyword& left, const keyword& right)
    {
        return left.value == right.value;
    }

    friend auto str(const keyword& k) -> string
    {
        return ':' + k.value;
    }

private:
    string value;
};

class list;
class vector;
struct function;
class expression;
struct environment;

using native_function = expression(*)(list const&);

using expression_variant = boost::variant<
    nil_type,
    symbol,
    keyword,
    string,
    integer,
    boolean,
    boost::recursive_wrapper<list>,
    boost::recursive_wrapper<vector>,
    native_function,
    boost::recursive_wrapper<function>
>;

class expression
{
public:
    expression() = default;
    expression(const nil_type& n) : value(n) { }
    expression(const symbol& s) : value(s) { }
    expression(const keyword& k) : value(k) { }
    expression(const string& s) : value(s) { }
    expression(const integer& i) : value(i) { }
    expression(const boolean& b) : value(b) { }
    expression(const list& l) : value(l) { }
    expression(const vector& v) : value(v) { }
    expression(native_function f) : value(f) { }
    expression(const function& f) : value(f) { }
    expression(const expression_variant& v) : value(v) { }

    template <typename result_type>
    struct visitor : boost::static_visitor<result_type> { };

    template <typename Visitor>
    friend auto apply(Visitor&& v, expression const& e)
    {
        return boost::apply_visitor(v, e.value);
    }

private:
    expression_variant value;

    friend auto operator==(const expression& left, const expression& right)
    {
        return left.value == right.value;
    }

    friend auto as_list(const expression& e) -> list const&
    {
        return boost::get<list>(e.value);
    }

    friend auto as_vector(const expression& e) -> vector const&
    {
        return boost::get<vector>(e.value);
    }

    friend auto as_symbol(const expression& e) -> symbol const&
    {
        return boost::get<symbol>(e.value);
    }

    friend auto as_integer(const expression& e) -> integer
    {
        return boost::get<integer>(e.value);
    }

    friend auto as_function(const expression& e) -> native_function
    {
        return boost::get<native_function>(e.value);
    }
};

inline auto operator!=(const expression& left, const expression& right)
{
    return !(left == right);
}

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

class vector : private list
{
public:
    using list::list;

    vector() = default;
    explicit vector(const list& l) : list(l) { }

    friend auto begin(const vector& v)
    {
        return begin(static_cast<const list&>(v));
    }

    friend auto end(const vector& v)
    {
        return end(static_cast<const list&>(v));
    }

    friend auto count(const vector& v) -> integer
    {
        return count(static_cast<const list&>(v));
    }

    friend auto operator==(const vector& left, const vector& right)
    {
        return static_cast<const list&>(left) == static_cast<const list&>(right);
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
