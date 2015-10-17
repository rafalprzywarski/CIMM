#include "eval_test.hpp"

namespace cimm
{

struct keyword_test : eval_test { };

TEST_F(keyword_test, should_make_keywords_from_stringlike_expressions)
{
    EXPECT_EQ(expression(keyword("a")), evaluate(list{symbol("keyword"), string("a")}));
    EXPECT_EQ(expression(keyword("a")), evaluate(list{symbol("keyword"), keyword("a")}));
    EXPECT_EQ(nil, evaluate(list{symbol("keyword"), nil}));
    EXPECT_EQ(nil, evaluate(list{symbol("keyword"), integer(5)}));
}

TEST_F(keyword_test, should_make_keywords_from_the_first_parameter_only)
{
    EXPECT_EQ(expression(keyword("a")), evaluate(list{symbol("keyword"), string("a"), string("b")}));
}

TEST_F(keyword_test, should_fail_for_no_parameters)
{
    assert_arity_error(0, "keyword", "(keyword)");
}

}
