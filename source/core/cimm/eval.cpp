#include "eval.hpp"

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

expression evaluate(environment&, const list& l)
{
    return add_integers(l);
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
