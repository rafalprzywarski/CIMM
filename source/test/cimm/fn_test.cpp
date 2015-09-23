#include "eval_test.hpp"
#include <cimm/parse.hpp>

namespace cimm
{

struct fn_test : eval_test
{
    auto evaluate_parsed(const string& expr)
    {
        return evaluate(parse_expression(expr));
    }
};

TEST_F(fn_test, should_evaluate_functions_without_arguments)
{
    EXPECT_EQ(integer(10), evaluate_parsed("((fn [] 10))"));
    EXPECT_EQ(integer(3), evaluate_parsed("((fn [] (+ 1 2)))"));
}

TEST_F(fn_test, should_evaluate_functions_with_one_argument)
{
    EXPECT_EQ(integer(3), evaluate_parsed("((fn [x] (+ x 1)) 2)"));
    EXPECT_EQ(integer(1), evaluate_parsed("((fn [x] (- x 1)) 2)"));
    EXPECT_EQ(integer(3), evaluate_parsed("((fn [x] (+ 1 x)) 2)"));
    EXPECT_EQ(integer(7), evaluate_parsed("((fn [x] (+ x x 1 x)) 2)"));
    EXPECT_EQ(integer(7), evaluate_parsed("((fn [f] (f 3 4)) +)"));
}

TEST_F(fn_test, should_evaluate_functions_with_multiple_arguments)
{
    EXPECT_EQ(integer(3), evaluate_parsed("((fn [x y] (+ x y)) -3 6)"));
    EXPECT_EQ(integer(18), evaluate_parsed("((fn [x y z] (+ x y z)) 3 6 9)"));
}

TEST_F(fn_test, should_evaluate_functions_with_symbols_as_body)
{
    evaluate_parsed("(def A 11)");
    EXPECT_EQ(integer(11), evaluate_parsed("((fn [] A))"));
    EXPECT_EQ(integer(32), evaluate_parsed("((fn [x] x) 32)"));
    EXPECT_EQ(integer(1), evaluate_parsed("((fn [x y z] x) 1 2 3)"));
    EXPECT_EQ(integer(2), evaluate_parsed("((fn [x y z] y) 1 2 3)"));
    EXPECT_EQ(integer(3), evaluate_parsed("((fn [x y z] z) 1 2 3)"));
}

TEST_F(fn_test, replace_all_parameters_in_one_step)
{
    evaluate_parsed("(def add (fn [x y] (+ x y)))");
    EXPECT_ANY_THROW(evaluate_parsed("(add 2 'x)"));
    EXPECT_ANY_THROW(evaluate_parsed("(add 'y' 2)"));
}

}
