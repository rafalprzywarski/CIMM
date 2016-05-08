#include "eval_test.hpp"

namespace cimm
{

struct method_test : eval_test { };

TEST_F(method_test, calling_a_generic_method_with_no_definition_should_fail)
{
    ASSERT_EQ(nil, evaluate_parsed("(defgeneric method1 [x])"));
    assert_evaluation_error<no_matching_method_found_error>("no matching method 'method1' found", "(method1 5)");
}

}
