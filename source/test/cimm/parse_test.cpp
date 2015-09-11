#include <gtest/gtest.h>
#include <cimm/parse.hpp>

namespace cimm
{

TEST(parse_test, should_parse_a_sequence_of_characters_as_a_string)
{
    ASSERT_EQ(string("abc123"), parse_expression("abc123"));
}

}
