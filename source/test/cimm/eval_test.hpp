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

    void assert_arity_error(integer n, const string& from, const string& source)
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

    template <typename error_type>
    void assert_evaluation_error(const string& msg, const string& source)
    {
        try
        {
            auto result = evaluate_parsed(source);
            FAIL() << "expected an exception; got " << result << " instead; source: " << source;
        }
        catch (error_type const& e)
        {
            EXPECT_EQ(msg, e.what());
        }
        catch (std::exception const& e)
        {
            FAIL() << "unexpected exception with message: " << e.what();
        }
        catch (...)
        {
            FAIL() << "unknown exception";
        }
    }
};

}
