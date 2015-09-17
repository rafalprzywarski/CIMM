#include "eval.hpp"
#include "error.hpp"

namespace cimm
{

namespace
{

auto evaluate(environment& env, const list& l) -> expression
{
    auto name = boost::get<symbol>(l.value.at(0).value).value;
    auto f = env.functions.find(name);
    if (f == env.functions.end())
        throw undefined_function_error(name);
    return f->second(env, map(rest(l), [&env](auto const& a) { return evaluate_expression(env, a); }));
}

template <typename expression_type>
auto evaluate(environment&, const expression_type& e) -> expression
{
    return e;
}

}

auto evaluate_expression(environment& env, const expression& expr) -> expression
{
    return apply([&env] (auto& e) { return evaluate(env, e); }, expr);
}

}
