#include <gtest/gtest.h>
#include <cimm/str.hpp>
#include <cimm/expression.hpp>

namespace cimm
{

TEST(pr_str_test, should_just_return_the_given_symbol_name)
{
    ASSERT_EQ(string("abc123"), pr_str(expression(symbol("abc123"))));
}

TEST(pr_str_test, should_convert_the_given_integer_to_a_string)
{
    EXPECT_EQ(string("0"), pr_str(expression(integer(0))));
    EXPECT_EQ(string("31902"), pr_str(expression(integer(31902))));
    EXPECT_EQ(string("-78"), pr_str(expression(integer(-78))));
}

TEST(pr_str_test, should_convert_an_empty_list_to_a_pair_of_parens)
{
    EXPECT_EQ(string("()"), pr_str(expression(list{})));
}

TEST(pr_str_test, should_convert_a_list_of_expressions_to_strings_separated_by_single_space_inside_parens)
{
    EXPECT_EQ(string("(1 2 3)"), pr_str(expression(list{1, 2, 3})));
    EXPECT_EQ(string("(-5)"), pr_str(expression(list{-5})));
    EXPECT_EQ(string("(((1) 2) (+ a) xyz)"), pr_str(expression(list{list{list{integer(1)}, integer(2)}, list{symbol("+"), symbol("a")}, symbol("xyz")})));
}

TEST(pr_str_test, should_convert_boolean_values)
{
    EXPECT_EQ("true", pr_str(expression(boolean(true))));
    EXPECT_EQ("false", pr_str(expression(boolean(false))));
}

TEST(pr_str_test, should_put_strings_in_quotes)
{
    ASSERT_EQ(string("\"abc123\""), pr_str(expression(string("abc123"))));
}

TEST(pr_str_test, should_escape_newline_characters)
{
    ASSERT_EQ(string("\"\\na\\nb\\nc\\n\""), pr_str(expression(string("\na\nb\nc\n"))));
}

TEST(pr_str_test, should_escape_quotes)
{
    ASSERT_EQ(string("\"\\\"a\\\"\""), pr_str(expression(string("\"a\""))));
}

TEST(pr_str_test, should_escape_backslashes)
{
    ASSERT_EQ(string("\"\\\\a\\\\\""), pr_str(expression(string("\\a\\"))));
}

TEST(pr_str_test, should_convert_nil_to_a_string)
{
    ASSERT_EQ(string("nil"), pr_str(expression(nil)));
}

TEST(pr_str_test, should_convert_keywords)
{
    ASSERT_EQ(string(":abcd"), pr_str(expression(keyword("abcd"))));
}

TEST(pr_str_test, should_convert_an_empty_list_to_a_pair_of_brackets)
{
    EXPECT_EQ(string("[]"), pr_str(expression(vector{})));
}

TEST(pr_str_test, should_convert_a_vector_of_expressions_to_strings_separated_by_single_space_inside_brackets)
{
    EXPECT_EQ(string("[1 2 3]"), pr_str(expression(vector{1, 2, 3})));
    EXPECT_EQ(string("[-5]"), pr_str(expression(vector{-5})));
    EXPECT_EQ(string("[[(1) 2] (+ a) xyz]"), pr_str(expression(vector{vector{list{integer(1)}, integer(2)}, list{symbol("+"), symbol("a")}, symbol("xyz")})));
}

TEST(pr_str_test, should_quote_strings_in_vectors)
{
    EXPECT_EQ(string("[\"abc\"]"), pr_str(expression{vector{string{"abc"}}}));
}

TEST(pr_str_test, should_quote_strings_in_lists)
{
    EXPECT_EQ(string("(\"abc\")"), pr_str(expression{list{string{"abc"}}}));
}

TEST(pr_str_test, should_convert_native_function_as_text_function)
{
    native_function f{string("some"), [](const list&) -> expression { return nil; }};
    EXPECT_EQ("fn", pr_str(expression(f)));
}

TEST(pr_str_test, should_convert_functions)
{
    function f{};
    EXPECT_EQ("fn", pr_str(f));
}

TEST(pr_str_test, should_convert_errors)
{
    EXPECT_EQ(string{"error: \"bad\""}, pr_str(error{expression{string{"bad"}}}));
    EXPECT_EQ(string{"error: 7"}, pr_str(error{expression{integer(7)}}));
}

}
