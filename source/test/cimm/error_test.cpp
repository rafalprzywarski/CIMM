#include "eval_test.hpp"

namespace cimm
{

struct error_test : eval_test { };

TEST_F(error_test, throw_should_wrap_an_expression_in_an_error)
{
    EXPECT_EQ(error{expression{integer(5)}}, evaluate_parsed("(throw 5)"));
    EXPECT_EQ((error{expression{list{integer(1), integer(2), integer(3)}}}), evaluate_parsed("(throw '(1 2 3))"));
}

TEST_F(error_test, calling_an_error_should_return_the_same_error)
{
    EXPECT_EQ(evaluate_parsed("(throw 5)"), evaluate_parsed("((throw 5))"));
    EXPECT_EQ(evaluate_parsed("(throw 5)"), evaluate_parsed("((throw 5) :a :b)"));
}

TEST_F(error_test, passing_an_error_as_an_argument_to_a_function_should_return_the_same_error)
{
    EXPECT_EQ(evaluate_parsed("(throw 5)"), evaluate_parsed("(+ (throw 5) 1 2)"));
    EXPECT_EQ(evaluate_parsed("(throw 5)"), evaluate_parsed("(+ 1 (throw 5) 2)"));
    EXPECT_EQ(evaluate_parsed("(throw 5)"), evaluate_parsed("(+ 1 2 (throw 5))"));
    EXPECT_EQ(evaluate_parsed("(throw 5)"), evaluate_parsed("(+ 1 (+ 2 (throw 5)))"));
}

TEST_F(error_test, catch_should_pass_the_unwrapped_error_expression_to_the_handler)
{
    EXPECT_EQ(integer(8), evaluate_parsed("(catch (throw 5) (fn [e] (+ e 3)))"));
}

TEST_F(error_test, catch_should_pass_through_if_no_error_was_returned)
{
    EXPECT_EQ(integer(5), evaluate_parsed("(catch 5 (fn [e] (+ e 3)))"));
    EXPECT_EQ(keyword("test"), evaluate_parsed("(catch :test (fn [e] (+ e 3)))"));
}

}
