#include <gtest/gtest.h>
#include <cimm/eval.hpp>
#include "expression_ostream.hpp"

namespace cimm
{

TEST(eval_test, should_add_integers)
{
    auto env = create_environment();
    EXPECT_EQ(integer(7), evaluate_expression(env, list{string("+"), integer(2), integer(5)}));
    EXPECT_EQ(integer(3), evaluate_expression(env, list{string("+"), integer(4), integer(-1)}));
    EXPECT_EQ(integer(6), evaluate_expression(env, list{string("+"), integer(1), integer(2), integer(3)}));
    EXPECT_EQ(integer(3), evaluate_expression(env, list{string("+"), integer(3)}));
}

TEST(eval_test, should_subtract_integers)
{
    auto env = create_environment();
    EXPECT_EQ(integer(-3), evaluate_expression(env, list{string("-"), integer(2), integer(5)}));
    EXPECT_EQ(integer(5), evaluate_expression(env, list{string("-"), integer(4), integer(-1)}));
}

}
