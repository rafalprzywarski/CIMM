#include "eval.hpp"
#include "error.hpp"

namespace cimm
{

namespace
{

expression add_integers(const list& l)
{
    integer sum{0};
    auto e = std::begin(l.value);
    while (++e != std::end(l.value))
      sum += boost::get<integer>(e->value);
    return sum;
}

expression subtract_integers(const list& l)
{
    return boost::get<integer>(l.value.at(1).value) - boost::get<integer>(l.value.at(2).value);
}

expression evaluate(environment& env, const list& l)
{
    if (l.value.at(0) == string("+"))
        return add_integers(l);
    else if (l.value.at(0) == string("-"))
        return subtract_integers(l);
    else
    {
        auto f = env.functions.find(boost::get<string>(l.value.at(0).value));
        if (f != env.functions.end())
            return f->second(env, l);
    }
    throw undefined_function_error(boost::get<string>(l.value.at(0).value));
}

template <typename expression_type>
expression evaluate(environment&, const expression_type& e)
{
    return e;
}

}

expression evaluate_expression(environment& env, const expression& expr)
{
    return apply([&env] (auto& e) { return evaluate(env, e); }, expr);
}

}
