#include <gtest/gtest.h>
#include <cimm/parse.hpp>
#include "expression_ostream.hpp"

namespace cimm
{

struct parse_test : testing::Test
{
    void assert_parse_error(const string& msg, const string& source)
    {
        try
        {
            auto result = parse_expression(source);
            FAIL() << "expected an exception; got " << result << " instead; source: " << source;
        }
        catch (parse_error const& e)
        {
            EXPECT_EQ(msg, e.what());
        }
        catch (std::exception const& e)
        {
            FAIL() << "unexpected exception with message: " << e.what();
        }
        catch (...)
        {
            FAIL() << "unknown exception";
        }
    }
};

TEST_F(parse_test, should_parse_a_sequence_of_characters_as_an_symbol)
{
    EXPECT_EQ(expression(symbol("abc123")), parse_expression("abc123"));
    EXPECT_EQ(expression(symbol("nil0")), parse_expression("nil0"));
    EXPECT_EQ(expression(symbol("true0")), parse_expression("true0"));
    EXPECT_EQ(expression(symbol("false0")), parse_expression("false0"));
}

TEST_F(parse_test, should_not_treat_newline_as_part_of_symbol)
{
    EXPECT_EQ(expression(symbol("abc123")), parse_expression("abc123\n"));
}

TEST_F(parse_test, should_parse_a_sequence_of_digits_as_an_integer)
{
    EXPECT_EQ(expression(integer(1)), parse_expression("1"));
    EXPECT_EQ(expression(integer(23)), parse_expression("23"));
    EXPECT_EQ(expression(integer(32134)), parse_expression("32134"));
}

TEST_F(parse_test, should_parse_negative_integers)
{
    EXPECT_EQ(expression(integer(-58)), parse_expression("-58"));
}

TEST_F(parse_test, should_parse_an_empty_list)
{
    EXPECT_EQ(expression(list{}), parse_expression("()"));
    EXPECT_EQ(expression(list{}), parse_expression("( )"));
}

TEST_F(parse_test, should_parse_a_list_of_expressions)
{
    EXPECT_EQ(expression(list{integer(1)}), parse_expression("(1)"));
    EXPECT_EQ(expression(list{symbol("+"), symbol("abc"), integer(-3)}), parse_expression("(+ abc -3)"));
    EXPECT_EQ(expression(list{list{list{symbol("x"), integer(-3)}, integer(1)}, list{integer(7)}}), parse_expression("(((x -3) 1) (7))"));
    EXPECT_EQ(expression(list{keyword("x")}), parse_expression("(:x)"));
}

TEST_F(parse_test, should_parse_boolean_values)
{
    EXPECT_EQ(expression(boolean(true)), parse_expression("true"));
    EXPECT_EQ(expression(boolean(true)), parse_expression("true "));
    EXPECT_EQ(expression(list{boolean(true)}), parse_expression("(true)"));
    EXPECT_EQ(expression(vector{boolean(true)}), parse_expression("[true]"));
    EXPECT_EQ(expression(boolean(false)), parse_expression("false"));
    EXPECT_EQ(expression(boolean(false)), parse_expression("false "));
    EXPECT_EQ(expression(list{boolean(false)}), parse_expression("(false)"));
    EXPECT_EQ(expression(vector{boolean(false)}), parse_expression("[false]"));
}

TEST_F(parse_test, should_parse_quoted_strings)
{
    EXPECT_EQ(expression(string("")), parse_expression("\"\""));
    EXPECT_EQ(expression(string("  ")), parse_expression("\"  \""));
    EXPECT_EQ(expression(string("abc123")), parse_expression("\"abc123\""));
    EXPECT_EQ(expression(list{string("a"), string("b")}), parse_expression("(\"a\" \"b\")"));
}
TEST_F(parse_test, should_not_treat_strings_as_parts_of_symbols)
{
    EXPECT_EQ(expression(list{symbol("ab"), string("cd")}), parse_expression("(ab\"cd\")"));
}

TEST_F(parse_test, should_parse_escaped_newline_in_strings)
{
    EXPECT_EQ(expression(string("ab\ncd")), parse_expression("\"ab\\ncd\""));
    EXPECT_EQ(expression(string("\n\n\n")), parse_expression("\"\\n\\n\\n\""));
}

TEST_F(parse_test, should_parse_escaped_backslashes_in_strings)
{
    EXPECT_EQ(expression(string("ab\\cd")), parse_expression("\"ab\\\\cd\""));
    EXPECT_EQ(expression(string("\\\\\\")), parse_expression("\"\\\\\\\\\\\\\""));
}

TEST_F(parse_test, should_parse_escaped_quotes_in_strings)
{
    EXPECT_EQ(expression(string("ab\"cd")), parse_expression("\"ab\\\"cd\""));
    EXPECT_EQ(expression(string("\"\"\"")), parse_expression("\"\\\"\\\"\\\"\""));
}

TEST_F(parse_test, should_parse_a_sequence_of_characters_beginning_with_a_colon_as_a_keyword)
{
    EXPECT_EQ(expression(keyword("abc123")), parse_expression(":abc123"));
    EXPECT_EQ(expression(list{keyword("abc123"), keyword("s")}), parse_expression("(:abc123 :s)"));
}

TEST_F(parse_test, should_parse_an_apostrophe_as_quote)
{
    EXPECT_EQ(expression(list{symbol("quote"), integer(7)}), parse_expression("'7"));
    EXPECT_EQ(expression(list{symbol("quote"), integer(7)}), parse_expression("' 7"));
    EXPECT_EQ(expression(list{symbol("quote"), symbol("abc123")}), parse_expression("'abc123"));
    EXPECT_EQ(expression(list{symbol("quote"), list{integer(1)}}), parse_expression("'(1)"));
    EXPECT_EQ(expression(list{symbol("quote"), list{integer(1)}}), parse_expression("' (1)"));
}

TEST_F(parse_test, should_parse_an_empty_vector)
{
    EXPECT_EQ(expression(vector{}), parse_expression("[]"));
    EXPECT_EQ(expression(vector{}), parse_expression("[ ]"));
}

TEST_F(parse_test, should_parse_a_vector_of_expressions)
{
    EXPECT_EQ(expression(vector{integer(1)}), parse_expression("[1]"));
    EXPECT_EQ(expression(vector{symbol("+"), symbol("abc"), integer(-3)}), parse_expression("[+ abc -3]"));
    EXPECT_EQ(expression(vector{vector{list{symbol("x"), integer(-3)}, integer(1)}, list{integer(7)}}), parse_expression("[[(x -3) 1] (7)]"));
    EXPECT_EQ(expression(vector{keyword("x")}), parse_expression("[:x]"));
}

TEST_F(parse_test, should_fail_when_parsing_an_unmatched_closing_bracket)
{
    assert_parse_error("unexpected ]", "]");
}

TEST_F(parse_test, should_fail_when_parsing_an_unmatched_closing_paren)
{
    assert_parse_error("unexpected )", ")");
}

TEST_F(parse_test, should_parse_nil)
{
    EXPECT_EQ(nil, parse_expression("nil"));
    EXPECT_EQ(nil, parse_expression("nil "));
    EXPECT_EQ(list{nil}, parse_expression("(nil)"));
    EXPECT_EQ(vector{nil}, parse_expression("[nil]"));
}

TEST_F(parse_test, should_parse_multiple_expressions)
{
    EXPECT_EQ((vector{list{nil, nil}, vector{integer(1), nil}}), parse_expressions("(nil nil) [1 nil]"));
    EXPECT_EQ((vector{list{nil, nil}, vector{integer(1), nil}}), parse_expressions("(nil nil)\n[1 nil]"));
}

TEST_F(parse_test, should_fail_when_missing_a_closing_paren)
{
    assert_parse_error("unexpected end of input", "(");
    assert_parse_error("unexpected end of input", "( 5 ");
    assert_parse_error("unexpected end of input", "(()");
}

TEST_F(parse_test, should_fail_when_missing_a_closing_bracket)
{
    assert_parse_error("unexpected end of input", "[");
    assert_parse_error("unexpected end of input", "[ 5 ");
    assert_parse_error("unexpected end of input", "[[]");
}

TEST_F(parse_test, should_fail_when_missing_a_closing_quote)
{
    assert_parse_error("unexpected end of input", "\"");
}

}
