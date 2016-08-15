#include <cimm/persistent_vector.hpp>
#include <atomic>
#include <gtest/gtest.h>

namespace cimm
{

struct persistent_vector_test : testing::Test
{
    struct traced_string
    {
        int *counter;
        std::string value;
        traced_string(int& counter, std::string value) : counter(&counter), value(std::move(value)) { counter++; }
        traced_string(const traced_string& other) : counter(other.counter), value(other.value) { ++*counter; }
        ~traced_string() { --*counter; }
    };

    traced_string s(std::string value)
    {
        return traced_string{trace_count, std::move(value)};
    }

    using string_vector = persistent_vector<traced_string, 2>;
    int trace_count = 0;

    void TearDown() override
    {
        ASSERT_EQ(0, trace_count);
    }
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
    string_vector one = v.push_back(s("one"));
    ASSERT_FALSE(one.empty());
    ASSERT_EQ(1u, one.size());
    ASSERT_EQ("one", one.at(0).value);
    ASSERT_THROW(one.at(1), std::out_of_range);
}

}
