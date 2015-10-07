#include <gtest/gtest.h>
#include <cimm/eval.hpp>
#include <cimm/error.hpp>
#include <cimm/default_environment.hpp>
#include "expression_ostream.hpp"
#include <cimm/parse.hpp>

namespace cimm
{

struct eval_test : testing::Test
{
    environment env = create_default_environment();

    auto evaluate(const expression& expr)
    {
        return evaluate_expression(env, expr);
    }

    auto evaluate_parsed(const string& expr)
    {
        return evaluate(parse_expression(expr));
    }

    auto parse(const string& expr)
    {
        return parse_expression(expr);
    }

    void assertArityError(integer n, const string& from, const string& source)
    {
        try
        {
            evaluate_parsed(source);
            FAIL() << "expected an exception; source: " << source;
        }
        catch (arity_error& e)
        {
            EXPECT_EQ("Wrong number of args (" + str(n) + ") passed to: " + from, e.what());
        }
    }
};

}
