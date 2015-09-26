#include <gtest/gtest.h>
#include <cimm/parse.hpp>
#include "expression_ostream.hpp"

namespace cimm
{

TEST(parse_test, should_parse_a_sequence_of_characters_as_an_symbol)
{
    ASSERT_EQ(expression(symbol("abc123")), parse_expression("abc123"));
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
    EXPECT_EQ(expression(list{symbol("+"), symbol("abc"), integer(-3)}), parse_expression("(+ abc -3)"));
    EXPECT_EQ(expression(list{list{list{symbol("x"), integer(-3)}, integer(1)}, list{integer(7)}}), parse_expression("(((x -3) 1) (7))"));
    EXPECT_EQ(expression(list{keyword("x")}), parse_expression("(:x)"));
}

TEST(parse_test, should_parse_boolean_values)
{
    EXPECT_EQ(expression(boolean(true)), parse_expression("true"));
    EXPECT_EQ(expression(boolean(false)), parse_expression("false"));
    EXPECT_EQ(expression(list{boolean(true), boolean(false)}), parse_expression("(true false)"));
}

TEST(parse_test, should_parse_quoted_strings)
{
    EXPECT_EQ(expression(string("")), parse_expression("\"\""));
    EXPECT_EQ(expression(string("  ")), parse_expression("\"  \""));
    EXPECT_EQ(expression(string("abc123")), parse_expression("\"abc123\""));
    EXPECT_EQ(expression(list{string("a"), string("b")}), parse_expression("(\"a\" \"b\")"));
}

TEST(parse_test, should_parse_a_sequence_of_characters_beginning_with_a_colon_as_a_keyword)
{
    EXPECT_EQ(expression(keyword("abc123")), parse_expression(":abc123"));
    EXPECT_EQ(expression(list{keyword("abc123"), keyword("s")}), parse_expression("(:abc123 :s)"));
}

TEST(parse_test, should_parse_an_apostrophe_as_quote)
{
    EXPECT_EQ(expression(list{symbol("quote"), integer(7)}), parse_expression("'7"));
    EXPECT_EQ(expression(list{symbol("quote"), integer(7)}), parse_expression("' 7"));
    EXPECT_EQ(expression(list{symbol("quote"), symbol("abc123")}), parse_expression("'abc123"));
    EXPECT_EQ(expression(list{symbol("quote"), list{integer(1)}}), parse_expression("'(1)"));
    EXPECT_EQ(expression(list{symbol("quote"), list{integer(1)}}), parse_expression("' (1)"));
}

TEST(parse_test, should_parse_an_empty_vector)
{
    EXPECT_EQ(expression(vector{}), parse_expression("[]"));
    EXPECT_EQ(expression(vector{}), parse_expression("[ ]"));
}

TEST(parse_test, should_parse_a_vector_of_expressions)
{
    EXPECT_EQ(expression(vector{integer(1)}), parse_expression("[1]"));
    EXPECT_EQ(expression(vector{symbol("+"), symbol("abc"), integer(-3)}), parse_expression("[+ abc -3]"));
    EXPECT_EQ(expression(vector{vector{list{symbol("x"), integer(-3)}, integer(1)}, list{integer(7)}}), parse_expression("[[(x -3) 1] (7)]"));
    EXPECT_EQ(expression(vector{keyword("x")}), parse_expression("[:x]"));
}

TEST(parse_test, should_parse_nil)
{
    EXPECT_EQ(nil, parse_expression("nil"));
}

}
