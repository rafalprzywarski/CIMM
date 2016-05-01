#include "eval_test.hpp"

namespace cimm
{

struct let_test : eval_test {};

TEST_F(let_test, should_define_local_constants)
{
    EXPECT_EQ(integer{7}, evaluate_parsed("(let [x 7] x)"));
    EXPECT_EQ(evaluate_parsed("'(1 2 3)"), evaluate_parsed("(let [a 1 b 2 c 3] (list a b c))"));
}

TEST_F(let_test, should_allow_for_no_bindings)
{
    EXPECT_EQ(integer{7}, evaluate_parsed("(let [] 7)"));
}

TEST_F(let_test, should_fail_for_odd_number_of_forms)
{
    assert_evaluation_error<let_forms_error>("let requires an even number of forms in binding vector", "(let [x] 1)");
    assert_evaluation_error<let_forms_error>("let requires an even number of forms in binding vector", "(let [x 4 y] 1)");
}

}
