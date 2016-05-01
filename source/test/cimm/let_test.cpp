#include "eval_test.hpp"

namespace cimm
{

struct let_test : eval_test {};

TEST_F(let_test, should_define_local_constants)
{
    EXPECT_EQ(integer{7}, evaluate_parsed("(let [x 7] x)"));
    EXPECT_EQ(evaluate_parsed("'(1 2 3)"), evaluate_parsed("(let [a 1 b 2 c 3] (list a b c))"));
}

TEST_F(let_test, should_allow_no_bindings)
{
    EXPECT_EQ(integer{7}, evaluate_parsed("(let [] 7)"));
}

TEST_F(let_test, should_allow_nesting)
{
    EXPECT_EQ(evaluate_parsed("'(1 2 3)"), evaluate_parsed("(let [a 1] (let [b 2] (let [c 3] (list a b c))))"));
}

TEST_F(let_test, nested_bindings_should_hide_parent_bindings)
{
    EXPECT_EQ(integer{2}, evaluate_parsed("(let [x 1] (let [x 2] x))"));
}

TEST_F(let_test, nested_bindings_should_be_able_to_use_parent_bindings)
{
    EXPECT_EQ(integer{2}, evaluate_parsed("(let [x 1] (let [y (+ 1 x)] y))"));
}

TEST_F(let_test, nested_bindings_should_be_able_to_use_parent_bindings_with_same_names)
{
    EXPECT_EQ(integer{2}, evaluate_parsed("(let [x 1] (let [x (+ 1 x)] x))"));
}

TEST_F(let_test, let_bindings_should_hide_function_bindings)
{
    EXPECT_EQ(integer{9}, evaluate_parsed("((fn [x] (let [x (+ x 2)] x)) 7)"));
}

TEST_F(let_test, function_bindings_should_hide_let_bindings)
{
    EXPECT_EQ(integer{9}, evaluate_parsed("(let [x 2] ((fn [x] (+ x 7)) x))"));
}

TEST_F(let_test, should_fail_for_odd_number_of_forms)
{
    assert_evaluation_error<let_forms_error>("let requires an even number of forms in binding vector", "(let [x] 1)");
    assert_evaluation_error<let_forms_error>("let requires an even number of forms in binding vector", "(let [x 4 y] 1)");
}

}
