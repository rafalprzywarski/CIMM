#include <gtest/gtest.h>
#include <cimm/str.hpp>
#include <cimm/expression.hpp>

namespace cimm
{

TEST(str_test, should_return_what_pr_str_returns)
{
    expression expr{list{vector{keyword{"x"}, symbol{"y"}, nil}}};
    ASSERT_EQ(pr_str(expr), str(expr));
}

TEST(str_test, should_just_return_the_given_string)
{
    ASSERT_EQ(string("abc123"), str(expression(string("abc123"))));
}

}
