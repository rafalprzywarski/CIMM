#include "eval_test.hpp"

namespace cimm
{

struct fn_test : eval_test { };

TEST_F(fn_test, should_evaluate_functions_without_arguments)
{
    EXPECT_EQ(integer(10), evaluate(list{list{special::fn, vector{}, integer(10)}}));
    EXPECT_EQ(integer(3), evaluate(list{list{special::fn, vector{}, list{symbol("+"), integer(1), integer(2)}}}));
}

}
