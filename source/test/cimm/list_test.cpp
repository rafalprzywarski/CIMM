#include "eval_test.hpp"

namespace cimm
{

struct list_test : eval_test { };

TEST_F(list_test, should_create_a_list)
{
    EXPECT_EQ(expression(list{}), evaluate(list{symbol("list")}));
    EXPECT_EQ(expression(list{integer(5)}), evaluate(list{symbol("list"), integer(5)}));
    EXPECT_EQ(expression(list{keyword("a"), string("b")}), evaluate(list{symbol("list"), keyword("a"), string("b")}));
}

TEST_F(list_test, first_should_provide_the_first_element)
{
    EXPECT_EQ(expression(keyword("a")), evaluate(list{symbol("first"), list{quote, list{keyword("a"), string("b")}}}));
}

TEST_F(list_test, first_should_return_nil_for_an_empty_list)
{
    EXPECT_EQ(expression(nil), evaluate(list{symbol("first"), list{quote, list{}}}));
}

TEST_F(list_test, first_should_return_nil_for_no_arguments)
{
    EXPECT_EQ(expression(nil), evaluate(list{symbol("first")}));
}

}
