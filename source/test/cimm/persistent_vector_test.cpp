#include <cimm/persistent_vector.hpp>
#include <atomic>
#include <gtest/gtest.h>

namespace cimm
{

struct persistent_vector_test : testing::Test
{
    using string_vector = persistent_vector<std::string>;
};

TEST_F(persistent_vector_test, default_constructor_should_create_an_empty_vector)
{
    string_vector empty;
    ASSERT_TRUE(empty.empty());
    ASSERT_EQ(0u, empty.size());
    ASSERT_THROW(empty.at(0), std::out_of_range);
    ASSERT_THROW(empty.at(1), std::out_of_range);
    ASSERT_TRUE(empty.begin() == empty.end());
}
}
