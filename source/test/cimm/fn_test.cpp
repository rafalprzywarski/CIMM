#include "eval_test.hpp"
#include <cimm/type_error.hpp>

namespace cimm
{

struct fn_test : eval_test { };

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

TEST_F(fn_test, should_not_evaluate_passed_lists)
{
    evaluate_parsed("(def id (fn [x] x))");
    EXPECT_EQ(list{}, evaluate_parsed("(id '())"));
    EXPECT_EQ((list{integer(1), integer(2)}), evaluate_parsed("(id '(1 2))"));
}

TEST_F(fn_test, should_apply_parameters_in_vectors)
{
    EXPECT_EQ(vector{}, evaluate_parsed("((fn [u v] []) 2 1)"));
    EXPECT_EQ(vector{integer(2)}, evaluate_parsed("((fn [u v] [u]) 2 1)"));
    EXPECT_EQ((vector{integer(1), integer(2)}), evaluate_parsed("((fn [u v] [v u]) 2 1)"));
}

TEST_F(fn_test, should_apply_parameters_in_vector_elements)
{
    EXPECT_EQ((vector{integer(7)}), evaluate_parsed("((fn [x] [(+ x 2)]) 5)"));
}

TEST_F(fn_test, should_not_apply_parameters_overridden_by_an_inner_function)
{
    EXPECT_EQ(integer(3), evaluate_parsed("((fn [x] (+ x ((fn [x] x) 1))) 2)"));
    EXPECT_EQ(integer(4), evaluate_parsed("((fn [x y] ((fn [x] (+ x y)) 1)) 2 3)"));
    EXPECT_EQ(
        integer(1 + 2 + 64 + 16 + 4 + 8),
        evaluate_parsed("((fn [x y z] (+ ((fn [x y] (+ x y z)) 1 2) ((fn [z y] (+ x y z)) 4 8))) 16 32 64)"));
    EXPECT_EQ(integer(6), evaluate_parsed("((fn [x y z] ((fn [x] ((fn [y] (+ x y z)) 2)) 1)) 9 8 3)"));
}

TEST_F(fn_test, should_not_apply_parameters_inside_quote)
{
    EXPECT_EQ(symbol("x"), evaluate_parsed("((fn [x] 'x) 5)"));
    EXPECT_EQ(list{symbol("x")}, evaluate_parsed("((fn [x] '(x)) 5)"));
}

TEST_F(fn_test, should_define_functions_overloaded_by_parameter_count)
{
    EXPECT_EQ(integer(3), evaluate_parsed("((fn ([x] x)) 3)"));
    EXPECT_EQ(integer(3), evaluate_parsed("((fn ([x] x) ([x y] (+ x y 7))) 3)"));
    EXPECT_EQ(integer(3 + 5 + 7), evaluate_parsed("((fn ([x] x) ([x y] (+ x y 7))) 3 5)"));
    EXPECT_EQ(integer(1), evaluate_parsed("((fn ([x] x) ([x y] y) ([x y z] z)) 1)"));
    EXPECT_EQ(integer(2), evaluate_parsed("((fn ([x] x) ([x y] y) ([x y z] z)) 1 2)"));
    EXPECT_EQ(integer(3), evaluate_parsed("((fn ([x] x) ([x y] y) ([x y z] z)) 1 2 3)"));
}

TEST_F(fn_test, should_fail_when_called_with_invalid_number_of_arguments)
{
    assert_arity_error(2, "fn", "((fn ([x] x)) 3 3)");
    assert_arity_error(3, "fn", "((fn ([x] x) ([x y] y)) 3 4 5)");
}

TEST_F(fn_test, should_evaluate_all_body_expressions_in_order)
{
    EXPECT_EQ(integer(3), evaluate_parsed("((fn ([x] (def a x) (def b a) (+ b 1))) 2)"));
}

TEST_F(fn_test, should_fail_when_calling_a_function_with_no_overloads)
{
    assert_arity_error(0, "fn", "((fn))");
    assert_arity_error(2, "fn", "((fn) 1 2)");
}

TEST_F(fn_test, should_fail_when_passed_nil_as_parameter_list)
{
    assert_evaluation_error<type_error>("nil is not a vector", "(fn nil)");
    assert_evaluation_error<type_error>("nil is not a vector", "(fn (nil))");
}

}
