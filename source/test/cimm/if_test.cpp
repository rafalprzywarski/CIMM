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
}

}
