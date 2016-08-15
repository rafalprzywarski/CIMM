#include <cimm/persistent_vector.hpp>
#include <atomic>
#include <gtest/gtest.h>

namespace cimm
{

struct persistent_vector_test : testing::Test
{
    using string_vector = persistent_vector<std::string, 2>;
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

TEST_F(persistent_vector_test, push_back_should_create_a_one_element_vector_from_en_empty_vector)
{
    string_vector v;
    string_vector one = v.push_back("one");
    ASSERT_FALSE(one.empty());
    ASSERT_EQ(1u, one.size());
    ASSERT_EQ("one", one.at(0));
    ASSERT_THROW(one.at(1), std::out_of_range);
}

}
