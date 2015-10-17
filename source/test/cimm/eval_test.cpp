#include "eval_test.hpp"
#include <cimm/type_error.hpp>

namespace cimm
{

TEST_F(eval_test, should_add_integers)
{
    EXPECT_EQ(integer(7), evaluate(list{symbol("+"), integer(2), integer(5)}));
    EXPECT_EQ(integer(3), evaluate(list{symbol("+"), integer(4), integer(-1)}));
    EXPECT_EQ(integer(6), evaluate(list{symbol("+"), integer(1), integer(2), integer(3)}));
    EXPECT_EQ(integer(3), evaluate(list{symbol("+"), integer(3)}));
}

TEST_F(eval_test, should_fail_when_trying_to_add_nonintegers)
{
    assert_evaluation_error<type_error>("a is not an integer", "(+ 'a 7)");
    ASSERT_THROW(evaluate_parsed("(+ :x)"), type_error);
    ASSERT_THROW(evaluate_parsed("(+ 1 3 :bad)"), type_error);
}

TEST_F(eval_test, should_negate_integers)
{
    EXPECT_EQ(integer(-2), evaluate_parsed("(- 2)"));
}

TEST_F(eval_test, should_subtract_integers)
{
    EXPECT_EQ(integer(-3), evaluate_parsed("(- 2 5)"));
    EXPECT_EQ(integer(5), evaluate_parsed("(- 4 -1)"));
    EXPECT_EQ(integer(-27), evaluate_parsed("(- 4 -1 12 20)"));
}

TEST_F(eval_test, should_fail_when_given_no_arguments_to_minus)
{
    assert_arity_error(0, "-", "(-)");
}

TEST_F(eval_test, should_fail_when_given_undefined_function)
{
    assert_evaluation_error<undefined_symbol_error>("undefined symbol \'bad\'", "(bad)");
}

TEST_F(eval_test, should_execute_functions_from_the_environment)
{
    environment env;
    auto func1 = [](const list& l) -> expression { return count(l); };
    auto func2 = [](const list& l) -> expression { return l; };

    define_native_function(env, symbol("func-1"), func1);
    define_native_function(env, symbol("func-2"), func2);

    EXPECT_EQ(integer(2), evaluate_expression(env, list{symbol("func-1"), integer(1), integer(2)}));
    EXPECT_EQ(expression(list{boolean(true), integer(2)}), evaluate_expression(env, list{symbol("func-2"), boolean(true), integer(2)}));
}

TEST_F(eval_test, should_execute_functions_with_specified_arity_from_the_environment)
{
    environment env;
    auto func0 = []() -> expression { return integer(7); };
    auto func1 = [](const expression& a) -> expression { return -as_integer(a); };
    auto func2 = [](const expression& a, const expression& b) -> expression { return as_integer(a) - as_integer(b); };

    define_native_function(env, symbol("func-0"), func0);
    define_native_function(env, symbol("func-1"), func1);
    define_native_function(env, symbol("func-2"), func2);

    EXPECT_EQ(integer(7), evaluate_expression(env, list{symbol("func-0")}));
    EXPECT_EQ(integer(-3), evaluate_expression(env, list{symbol("func-1"), integer(3)}));
    EXPECT_EQ(integer(-1), evaluate_expression(env, list{symbol("func-2"), integer(2), integer(3)}));
}

TEST_F(eval_test, should_fail_when_executing_a_functions_with_specified_arity_from_the_environment_with_mismatched_number_of_args)
{
    env = {};
    auto func0 = []() -> expression { return nil; };
    auto func1 = [](const expression& a) -> expression { return nil; };
    auto func2 = [](const expression& a, const expression& b) -> expression { return nil; };

    define_native_function(env, symbol("func-0"), func0);
    define_native_function(env, symbol("func-1"), func1);
    define_native_function(env, symbol("func-2"), func2);

    assert_arity_error(2, "func-0", "(func-0 10 11)");
    assert_arity_error(2, "func-1", "(func-1 10 11)");
    assert_arity_error(3, "func-2", "(func-2 10 11 12)");
}

TEST_F(eval_test, should_evaluate_function_arguments)
{
    environment env;
    define_native_function(env, symbol("id"), [](const list& l) -> expression { return first(l); });
    define_native_function(env, symbol("double"), [](const list& l) -> expression { return as_integer(first(l)) * integer(2); });
    define_native_function(env, symbol("first"), [](const list& l) -> expression { return first(l); });
    define_native_function(env, symbol("second"), [](const list& l) -> expression { return first(rest(l)); });

    EXPECT_EQ(integer(7), evaluate_expression(env, list{symbol("first"), list{symbol("id"), integer(7)}, list{symbol("double"), integer(10)}}));
    EXPECT_EQ(integer(20), evaluate_expression(env, list{symbol("second"), list{symbol("id"), integer(7)}, list{symbol("double"), integer(10)}}));
}

TEST_F(eval_test, should_check_equality_of_boolean_values)
{
    EXPECT_EQ(boolean(true), evaluate(list{symbol("="), boolean(true)}));
    EXPECT_EQ(boolean(true), evaluate(list{symbol("="), boolean(false)}));
    EXPECT_EQ(boolean(true), evaluate(list{symbol("="), boolean(false), boolean(false)}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("="), boolean(true), boolean(false)}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("="), boolean(true), boolean(false), boolean(false)}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("="), boolean(false), boolean(true), boolean(false)}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("="), boolean(false), boolean(false), boolean(true)}));
    EXPECT_EQ(boolean(true), evaluate(list{symbol("="), boolean(true), boolean(true), boolean(true)}));
}

TEST_F(eval_test, equality_should_fail_when_given_no_arguments)
{
    assert_arity_error(0, "=", "(=)");
}

TEST_F(eval_test, should_check_equality_of_integers)
{
    EXPECT_EQ(boolean(false), evaluate(list{symbol("="), integer(1), integer(2)}));
    EXPECT_EQ(boolean(true), evaluate(list{symbol("="), integer(3), integer(3)}));
}

TEST_F(eval_test, should_negate_expressions)
{
    EXPECT_EQ(boolean(false), evaluate(list{symbol("not"), boolean(true)}));
    EXPECT_EQ(boolean(true), evaluate(list{symbol("not"), boolean(false)}));
    EXPECT_EQ(boolean(true), evaluate(list{symbol("not"), nil}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("not"), integer(2)}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("not"), integer(0)}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("not"), string("abc")}));
    EXPECT_EQ(boolean(true), evaluate(list{symbol("not")}));
}

TEST_F(eval_test, should_quote)
{
    EXPECT_EQ(expression(list{integer(1), integer(2), integer(3)}), evaluate(list{special::quote, list{integer(1), integer(2), integer(3)}}));
}

TEST_F(eval_test, should_check_equality_of_lists)
{
    EXPECT_EQ(boolean(true), evaluate(list{symbol("="), list{special::quote, list{integer(1), boolean(true)}}, list{special::quote, list{integer(1), boolean(true)}}}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("="), list{special::quote, list{integer(2), boolean(true)}}, list{special::quote, list{integer(1), boolean(false)}}}));
}

TEST_F(eval_test, should_check_inequality_of_expressions)
{
    EXPECT_EQ(boolean(true), evaluate(list{symbol("not="), integer(1), integer(2)}));
    EXPECT_EQ(boolean(false), evaluate(list{symbol("not="), integer(3), integer(3)}));
}

TEST_F(eval_test, should_evaluate_elements_of_a_vector)
{
    EXPECT_EQ((vector{}), evaluate_parsed("[]"));
    EXPECT_EQ((vector{integer(3)}), evaluate_parsed("[(+ 1 2)]"));
    EXPECT_EQ((vector{integer(3), integer(4), integer(5)}), evaluate_parsed("[(+ 1 2) (- 6 2) (+ 2 3)]"));
}

}
