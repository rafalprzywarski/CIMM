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
    EXPECT_EQ(expression(keyword("a")), evaluate(list{symbol("first"), list{special::quote, list{keyword("a"), string("b")}}}));
}

TEST_F(list_test, first_should_return_nil_for_an_empty_list)
{
    EXPECT_EQ(expression(nil), evaluate(list{symbol("first"), list{special::quote, list{}}}));
}

TEST_F(list_test, first_should_return_nil_for_no_arguments)
{
    EXPECT_EQ(expression(nil), evaluate(list{symbol("first")}));
}

TEST_F(list_test, rest_should_return_an_empty_list_for_no_arguments)
{
    EXPECT_EQ(expression(list{}), evaluate(list{symbol("rest")}));
}

TEST_F(list_test, rest_should_return_the_list_of_elements_after_the_first_element)
{
    EXPECT_EQ(expression(list{}), evaluate(list{symbol("rest"), list{special::quote, list{integer(3)}}}));
    EXPECT_EQ(expression(list{integer(3)}), evaluate(list{symbol("rest"), list{special::quote, list{integer(1), integer(3)}}}));
    EXPECT_EQ(expression(list{integer(3), integer(7)}), evaluate(list{symbol("rest"), list{special::quote, list{integer(1), integer(3), integer(7)}}}));
}

TEST_F(list_test, first_should_return_an_empty_list_for_an_empty_list)
{
    EXPECT_EQ(expression(list{}), evaluate(list{symbol("rest")}));
}

TEST_F(list_test, first_should_return_an_empty_list_for_nil)
{
    EXPECT_EQ(expression(list{}), evaluate(list{symbol("rest"), nil}));
}

TEST_F(list_test, count_should_return_0_for_nil)
{
    EXPECT_EQ(expression(integer(0)), evaluate(list{symbol("count"), nil}));
}

TEST_F(list_test, count_should_return_0_for_no_arguments)
{
    EXPECT_EQ(expression(integer(0)), evaluate(list{symbol("count"), nil}));
}

TEST_F(list_test, count_should_return_the_number_of_elements_in_a_list)
{
    EXPECT_EQ(expression(integer(0)), evaluate(list{symbol("count"), list{special::quote, list{}}}));
    EXPECT_EQ(expression(integer(1)), evaluate(list{symbol("count"), list{special::quote, list{integer(4)}}}));
    EXPECT_EQ(expression(integer(2)), evaluate(list{symbol("count"), list{special::quote, list{integer(4), integer(3)}}}));
}

TEST_F(list_test, cons_should_add_an_element_in_front_of_the_list)
{
    EXPECT_EQ(expression(list{integer(5)}), evaluate(list{symbol("cons"), integer(5), list{special::quote, list{}}}));
    EXPECT_EQ(expression(list{integer(5), integer(7)}), evaluate(list{symbol("cons"), integer(5), list{special::quote, list{integer(7)}}}));
}

TEST_F(list_test, cons_should_return_a_list_with_one_nil_for_no_arguments)
{
    EXPECT_EQ(expression(list{nil}), evaluate(list{symbol("cons")}));
}

TEST_F(list_test, cons_should_return_a_list_with_given_element_when_a_list_is_not_given)
{
    EXPECT_EQ(expression(list{integer(5)}), evaluate(list{symbol("cons"), integer(5)}));
}

TEST_F(list_test, conj_should_add_an_element_in_front_of_the_list)
{
    EXPECT_EQ(expression(list{integer(5)}), evaluate(list{symbol("conj"), list{special::quote, list{}}, integer(5)}));
    EXPECT_EQ(expression(list{integer(5), integer(7)}), evaluate(list{symbol("conj"), list{special::quote, list{integer(7)}}, integer(5)}));
}

}
