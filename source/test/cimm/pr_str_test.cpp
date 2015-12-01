#include <gtest/gtest.h>
#include <cimm/str.hpp>
#include <cimm/expression.hpp>

namespace cimm
{

TEST(pr_str_test, should_put_strings_in_quotes)
{
    ASSERT_EQ(string("\"abc123\""), pr_str(expression(string("abc123"))));
}

}
