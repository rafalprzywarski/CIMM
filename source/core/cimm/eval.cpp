#include "eval.hpp"
#include "error.hpp"

namespace cimm
{

namespace
{

expression evaluate(environment& env, const list& l)
{
    auto name = boost::get<identifier>(l.value.at(0).value);
    auto f = env.functions.find(name);
    if (f == env.functions.end())
        throw undefined_function_error(boost::get<identifier>(l.value.at(0).value));
    return f->second(env, l);
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
