#include <gtest/gtest.h>
#include <cimm/str.hpp>

namespace cimm
{

TEST(str_test, should_just_return_the_given_symbol_name)
{
    ASSERT_EQ(string("abc123"), str(expression(symbol("abc123"))));
}

TEST(str_test, should_convert_the_given_integer_to_a_string)
{
    EXPECT_EQ(string("0"), str(expression(integer(0))));
    EXPECT_EQ(string("31902"), str(expression(integer(31902))));
    EXPECT_EQ(string("-78"), str(expression(integer(-78))));
}

TEST(str_test, should_convert_an_empty_list_to_a_pair_of_parens)
{
    EXPECT_EQ(string("()"), str(expression(list{})));
}

TEST(str_test, should_convert_a_list_of_expressions_to_strings_separated_by_single_space_inside_parens)
{
    EXPECT_EQ(string("(1 2 3)"), str(expression(list{1, 2, 3})));
    EXPECT_EQ(string("(-5)"), str(expression(list{-5})));
    EXPECT_EQ(string("(((1) 2) (+ a) xyz)"), str(expression(list{list{list{integer(1)}, integer(2)}, list{symbol("+"), symbol("a")}, symbol("xyz")})));
}

TEST(str_test, should_convert_boolean_values)
{
    EXPECT_EQ("true", str(expression(boolean(true))));
    EXPECT_EQ("false", str(expression(boolean(false))));
}

TEST(str_test, should_just_return_the_given_string)
{
    ASSERT_EQ(string("abc123"), str(expression(string("abc123"))));
}

TEST(str_test, should_convert_nil_to_a_string)
{
    ASSERT_EQ(string("nil"), str(expression(nil)));
}

TEST(str_test, should_convert_keywords)
{
    ASSERT_EQ(string(":abcd"), str(expression(keyword("abcd"))));
}

TEST(str_test, should_convert_an_empty_list_to_a_pair_of_brackets)
{
    EXPECT_EQ(string("[]"), str(expression(vector{})));
}

}
