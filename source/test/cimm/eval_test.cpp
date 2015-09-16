#include <gtest/gtest.h>
#include <cimm/eval.hpp>
#include <cimm/error.hpp>
#include "expression_ostream.hpp"

namespace cimm
{

struct eval_test : testing::Test
{
    environment env = create_environment();
};

TEST_F(eval_test, should_add_integers)
{
    EXPECT_EQ(integer(7), evaluate_expression(env, list{identifier("+"), integer(2), integer(5)}));
    EXPECT_EQ(integer(3), evaluate_expression(env, list{identifier("+"), integer(4), integer(-1)}));
    EXPECT_EQ(integer(6), evaluate_expression(env, list{identifier("+"), integer(1), integer(2), integer(3)}));
    EXPECT_EQ(integer(3), evaluate_expression(env, list{identifier("+"), integer(3)}));
}

TEST_F(eval_test, should_subtract_integers)
{
    EXPECT_EQ(integer(-3), evaluate_expression(env, list{identifier("-"), integer(2), integer(5)}));
    EXPECT_EQ(integer(5), evaluate_expression(env, list{identifier("-"), integer(4), integer(-1)}));
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
    auto func2 = [](environment&, const list& l) -> expression { return integer(l.value.size() * 7); };

    define_native_function(env, identifier("func-1"), func1);
    define_native_function(env, identifier("func-2"), func2);

    ASSERT_EQ(integer(3), evaluate_expression(env, list{identifier("func-1"), list{}, list{}}));
    ASSERT_EQ(integer(3 * 7), evaluate_expression(env, list{identifier("func-2"), list{}, list{}}));
}

}
