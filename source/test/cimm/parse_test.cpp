#include <gtest/gtest.h>
#include <cimm/parse.hpp>
#include "expression_ostream.hpp"

namespace cimm
{

TEST(parse_test, should_parse_a_sequence_of_characters_as_a_string)
{
    ASSERT_EQ(expression(string("abc123")), parse_expression("abc123"));
}

TEST(parse_test, should_parse_a_sequence_of_digits_as_an_integer)
{
    EXPECT_EQ(expression(integer(1)), parse_expression("1"));
    EXPECT_EQ(expression(integer(23)), parse_expression("23"));
    EXPECT_EQ(expression(integer(32134)), parse_expression("32134"));
}

TEST(parse_test, should_parse_negative_integers)
{
    EXPECT_EQ(expression(integer(-58)), parse_expression("-58"));
}

TEST(parse_test, should_parse_an_empty_list)
{
    EXPECT_EQ(expression(list{}), parse_expression("()"));
    EXPECT_EQ(expression(list{}), parse_expression("( )"));
}

TEST(parse_test, should_parse_a_list_of_expressions)
{
    EXPECT_EQ(expression(list{integer(1)}), parse_expression("(1)"));
    EXPECT_EQ(expression(list{string("+"), string("abc"), integer(-3)}), parse_expression("(+ abc -3)"));
    EXPECT_EQ(expression(list{list{list{string("x"), integer(-3)}, integer(1)}, list{integer(7)}}), parse_expression("(((x -3) 1) (7))"));
}

}
