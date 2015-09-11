#include <gtest/gtest.h>
#include <cimm/parse.hpp>

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

}
