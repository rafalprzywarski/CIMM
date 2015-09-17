#include <gtest/gtest.h>
#include <cimm/eval.hpp>
#include <cimm/error.hpp>
#include <cimm/default_environment.hpp>
#include "expression_ostream.hpp"

namespace cimm
{

struct eval_test : testing::Test
{
    environment env = create_default_environment();

    auto evaluate(const expression& expr)
    {
        return evaluate_expression(env, expr);
    }
};

TEST_F(eval_test, should_add_integers)
{
    EXPECT_EQ(integer(7), evaluate(list{identifier("+"), integer(2), integer(5)}));
    EXPECT_EQ(integer(3), evaluate(list{identifier("+"), integer(4), integer(-1)}));
    EXPECT_EQ(integer(6), evaluate(list{identifier("+"), integer(1), integer(2), integer(3)}));
    EXPECT_EQ(integer(3), evaluate(list{identifier("+"), integer(3)}));
}

TEST_F(eval_test, should_subtract_integers)
{
    EXPECT_EQ(integer(-3), evaluate(list{identifier("-"), integer(2), integer(5)}));
    EXPECT_EQ(integer(5), evaluate(list{identifier("-"), integer(4), integer(-1)}));
}

TEST_F(eval_test, should_fail_when_given_undefined_function)
{
    try
    {
        evaluate_expression(env, list{identifier("bad")});
        FAIL() << "expected an evaluation error";
    }
    catch (undefined_function_error const& e)
    {
        ASSERT_STREQ("undefined function \'bad\'", e.what());
    }
}

TEST_F(eval_test, should_execute_functions_from_the_environment)
{
    environment env;
    auto func1 = [](environment&, const list& l) -> expression { return integer(l.value.size()); };
    auto func2 = [](environment&, const list& l) -> expression { return l; };

    define_native_function(env, identifier("func-1"), func1);
    define_native_function(env, identifier("func-2"), func2);

    EXPECT_EQ(integer(2), evaluate_expression(env, list{identifier("func-1"), list{}, list{}}));
    EXPECT_EQ(expression(list{boolean(true), list{}}), evaluate_expression(env, list{identifier("func-2"), boolean(true), list{}}));
}

TEST_F(eval_test, should_check_equality_of_boolean_values)
{
    EXPECT_EQ(boolean(true), evaluate(list{identifier("=")}));
    EXPECT_EQ(boolean(true), evaluate(list{identifier("="), boolean(true)}));
    EXPECT_EQ(boolean(true), evaluate(list{identifier("="), boolean(false)}));
    EXPECT_EQ(boolean(true), evaluate(list{identifier("="), boolean(false), boolean(false)}));
    EXPECT_EQ(boolean(false), evaluate(list{identifier("="), boolean(true), boolean(false)}));
    EXPECT_EQ(boolean(false), evaluate(list{identifier("="), boolean(true), boolean(false), boolean(false)}));
    EXPECT_EQ(boolean(false), evaluate(list{identifier("="), boolean(false), boolean(true), boolean(false)}));
    EXPECT_EQ(boolean(false), evaluate(list{identifier("="), boolean(false), boolean(false), boolean(true)}));
    EXPECT_EQ(boolean(true), evaluate(list{identifier("="), boolean(true), boolean(true), boolean(true)}));
}

TEST_F(eval_test, should_check_equality_of_integers)
{
    EXPECT_EQ(boolean(false), evaluate(list{identifier("="), integer(1), integer(2)}));
    EXPECT_EQ(boolean(true), evaluate(list{identifier("="), integer(3), integer(3)}));
}

TEST_F(eval_test, should_check_equality_of_lists)
{
    EXPECT_EQ(boolean(true), evaluate(list{identifier("="), list{integer(1), boolean(true)}, list{integer(1), boolean(true)}}));
    EXPECT_EQ(boolean(false), evaluate(list{identifier("="), list{integer(2), boolean(true)}, list{integer(1), boolean(false)}}));
}

}
