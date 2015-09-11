#include <gtest/gtest.h>
#include <cimm/str.hpp>

namespace cimm
{

TEST(str_test, should_just_return_the_given_string)
{
    ASSERT_EQ(string("abc123"), str(expression(string("abc123"))));
}

TEST(str_test, should_convert_the_given_integer_to_a_string)
{
    ASSERT_EQ(string("0"), str(expression(integer(0))));
    ASSERT_EQ(string("31902"), str(expression(integer(31902))));
    ASSERT_EQ(string("-78"), str(expression(integer(-78))));
}

}
