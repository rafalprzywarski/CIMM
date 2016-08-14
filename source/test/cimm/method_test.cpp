#include "eval_test.hpp"

namespace cimm
{

struct method_test : eval_test { };

TEST_F(method_test, calling_a_generic_method_with_no_definition_should_fail)
{
    ASSERT_EQ(nil, evaluate_parsed("(defgeneric method1 [x])"));
    assert_evaluation_error<no_matching_method_found_error>("no matching method 'method1' found", "(method1 5)");
}

TEST_F(method_test, should_call_the_defined_method)
{
    ASSERT_EQ(nil, evaluate_parsed("(defgeneric method1 [x y])"));
    ASSERT_EQ(nil, evaluate_parsed("(defmethod method1 [x y] (str x y \"!\"))"));
    ASSERT_EQ(string{"345!"}, evaluate_parsed("(method1 34 5)"));
}

}
