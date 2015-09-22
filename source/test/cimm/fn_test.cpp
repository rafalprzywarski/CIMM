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
}

}
