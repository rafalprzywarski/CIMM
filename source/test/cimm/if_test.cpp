#include "eval_test.hpp"

namespace cimm
{

struct if_test : eval_test { };

TEST_F(if_test, should_return_the_first_value_if_the_condition_is_true_without_evaluating_the_second_value)
{
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if true :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if 0 :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if 10 :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if \"\" :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if \"x\" :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if :x :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if [] :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if '() :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if (fn [] '()) :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if + :first (not-evaluated))"));
    EXPECT_EQ(parse(":first"), evaluate_parsed("(if + :first)"));
}

TEST_F(if_test, should_return_the_second_value_if_the_condition_is_false_without_evaluating_the_first_value)
{
    EXPECT_EQ(parse(":second"), evaluate_parsed("(if false (not-evaluated) :second)"));
    EXPECT_EQ(parse(":second"), evaluate_parsed("(if nil (not-evaluated) :second)"));
}

TEST_F(if_test, should_evaluate_returned_values)
{
    EXPECT_EQ(parse("5"), evaluate_parsed("(if true (+ 2 3) (not-evaluated))"));
    EXPECT_EQ(parse("3"), evaluate_parsed("(if false (not-evaluated) (+ 1 2))"));
}

TEST_F(if_test, should_return_nil_if_second_value_is_not_provided)
{
    EXPECT_EQ(nil, evaluate_parsed("(if false 5)"));
}

}
