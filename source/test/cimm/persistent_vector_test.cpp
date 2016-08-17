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

    using string_vector = persistent_vector<traced_string, 2>;
    int trace_count = 0;

    traced_string s(std::string value)
    {
        return traced_string{trace_count, std::move(value)};
    }

    std::vector<std::string> numbers(unsigned n)
    {
        std::vector<std::string> v;
        for (unsigned i = 1; i <= n; ++i)
            v.push_back(std::to_string(i));
        return v;
    }

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

TEST_F(persistent_vector_test, push_back_should_create_a_one_element_vector_from_an_empty_vector)
{
    string_vector v;
    string_vector one = v.push_back(s("one"));
    ASSERT_FALSE(one.empty());
    ASSERT_EQ(1u, one.size());
    ASSERT_EQ("one", one.at(0).value);
    ASSERT_THROW(one.at(1), std::out_of_range);
}

TEST_F(persistent_vector_test, push_back_should_create_a_vector_with_an_element_appended_at_the_end_of_a_given_vector_within_one_leaf)
{
    string_vector two = string_vector{}.push_back(s("one")).push_back(s("two"));
    string_vector three = two.push_back(s("three"));
    string_vector four = three.push_back(s("four"));
    ASSERT_FALSE(two.empty());
    ASSERT_EQ(2u, two.size());
    ASSERT_EQ("one", two.at(0).value);
    ASSERT_EQ("two", two.at(1).value);
    ASSERT_THROW(two.at(2), std::out_of_range);

    ASSERT_EQ(3u, three.size());
    ASSERT_EQ("one", three.at(0).value);
    ASSERT_EQ("two", three.at(1).value);
    ASSERT_EQ("three", three.at(2).value);
    ASSERT_THROW(three.at(3), std::out_of_range);

    ASSERT_EQ(4u, four.size());
    ASSERT_EQ("one", four.at(0).value);
    ASSERT_EQ("four", four.at(3).value);
    ASSERT_THROW(four.at(4), std::out_of_range);
}

TEST_F(persistent_vector_test, push_back_should_create_a_vector_with_an_element_appended_at_the_end_of_a_given_vector)
{
    std::vector<std::string> elems = numbers(4 * 4 * 4 * 4 * 4);
    for (std::size_t n = 5; n <= elems.size(); ++n)
    {
        try
        {
            string_vector vec;
            for (std::size_t i = 0; i < (n - 1); ++i)
                vec = vec.push_back(s(elems[i]));

            string_vector appended = vec.push_back(s(elems[n - 1]));
            EXPECT_EQ(n - 1, vec.size());
            for (std::size_t i = 0; i < (n - 1); ++i)
                EXPECT_EQ(elems[i], vec.at(i).value) << "at: " << i << " size: " << n;

            EXPECT_THROW(vec.at(n - 1), std::out_of_range);

            EXPECT_EQ(n, appended.size());
            for (std::size_t i = 0; i < n; ++i)
                EXPECT_EQ(elems[i], appended.at(i).value) << "at: " << i << " size: " << n;

            EXPECT_THROW(appended.at(n), std::out_of_range);
        }
        catch (const std::exception& e)
        {
            ADD_FAILURE() << "n = " << n << ", exception: " << e.what();
        }
    }
}

}
