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

TEST_F(vector_test, first_should_provide_the_first_element)
{
    EXPECT_EQ(parse(":a"), evaluate_parsed("(first [:a \"b\"])"));
}

TEST_F(vector_test, first_should_return_nil_for_an_empty_vector)
{
    EXPECT_EQ(nil, evaluate_parsed("(first [])"));
}

TEST_F(vector_test, conj_should_add_an_element_at_the_end_of_the_vector)
{
    EXPECT_EQ(parse("[5]"), evaluate_parsed("(conj [] 5)"));
    EXPECT_EQ(parse("[5 7]"), evaluate_parsed("(conj [5] 7)"));
    EXPECT_EQ(parse("[5 7 9]"), evaluate_parsed("(conj [5 7] 9)"));
}

}
