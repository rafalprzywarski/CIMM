#include "eval_test.hpp"

namespace cimm
{

struct vector_test : eval_test { };

TEST_F(vector_test, should_create_a_vector)
{
    EXPECT_EQ(parse("[]"), evaluate_parsed("(vector)"));
    EXPECT_EQ(parse("[5]"), evaluate_parsed("(vector 5)"));
    EXPECT_EQ(parse("[:a \"b\"]"), evaluate_parsed("(vector :a \"b\")"));
}

TEST_F(vector_test, should_create_a_vector_from_a_list)
{
    EXPECT_EQ(parse("[]"), evaluate_parsed("(vec '())"));
    EXPECT_EQ(parse("[5]"), evaluate_parsed("(vec '(5))"));
    EXPECT_EQ(parse("[:a \"b\"]"), evaluate_parsed("(vec '(:a \"b\"))"));
}

}
