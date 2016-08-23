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

    std::vector<traced_string> numbers(unsigned n)
    {
        std::vector<traced_string> v;
        for (unsigned i = 1; i <= n; ++i)
            v.push_back(s(std::to_string(i)));
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
    auto elems = numbers(4 * 4 * 4 * 4 * 4);
    for (std::size_t n = 5; n <= elems.size(); ++n)
    {
        try
        {
            string_vector vec;
            for (std::size_t i = 0; i < (n - 1); ++i)
                vec = vec.push_back(elems[i]);

            string_vector appended = vec.push_back(elems[n - 1]);
            EXPECT_EQ(n - 1, vec.size());
            for (std::size_t i = 0; i < (n - 1); ++i)
                EXPECT_EQ(elems[i].value, vec.at(i).value) << "at: " << i << " size: " << n;

            EXPECT_THROW(vec.at(n - 1), std::out_of_range);

            EXPECT_EQ(n, appended.size());
            for (std::size_t i = 0; i < n; ++i)
                EXPECT_EQ(elems[i].value, appended.at(i).value) << "at: " << i << " size: " << n;

            EXPECT_THROW(appended.at(n), std::out_of_range);
        }
        catch (const std::exception& e)
        {
            ADD_FAILURE() << "n = " << n << ", exception: " << e.what();
        }
    }
}

TEST_F(persistent_vector_test, should_provide_initializer_list_constructor)
{
    auto empty = string_vector{{}};
    EXPECT_TRUE(empty.empty());

    auto two = string_vector{s("one"), s("two")};
    EXPECT_EQ(2u, two.size());
    EXPECT_EQ("one", two.at(0).value);
    EXPECT_EQ("two", two.at(1).value);
}

TEST_F(persistent_vector_test, should_provide_iterator_pair_constructor)
{
    std::vector<traced_string> emptyContainer;
    auto empty = string_vector{begin(emptyContainer), end(emptyContainer)};
    EXPECT_TRUE(empty.empty());

    std::vector<traced_string> twoContainer{s("one"), s("two")};
    auto two = string_vector{begin(twoContainer), end(twoContainer)};
    EXPECT_EQ(2u, two.size());
    EXPECT_EQ("one", two.at(0).value);
    EXPECT_EQ("two", two.at(1).value);
}

TEST_F(persistent_vector_test, should_provide_indexing_operator)
{
    auto two = string_vector{s("one"), s("two")};
    EXPECT_EQ(2u, two.size());
    EXPECT_EQ("one", two[0].value);
    EXPECT_EQ("two", two[1].value);
}

TEST_F(persistent_vector_test, should_provide_front)
{
    EXPECT_EQ("one", (string_vector{s("one")}.front().value));
    EXPECT_EQ("one", (string_vector{s("one"), s("two")}.front().value));
}

TEST_F(persistent_vector_test, should_provide_back)
{
    EXPECT_EQ("one", (string_vector{s("one")}.back().value));
    EXPECT_EQ("two", (string_vector{s("one"), s("two")}.back().value));
}

TEST_F(persistent_vector_test, should_provide_random_access_iterators)
{
    auto n = numbers(4 * 4 - 1);
    string_vector v{begin(n), end(n)};
    ASSERT_EQ(v.size(), v.end() - v.begin());
    for (string_vector::size_type i = 0; i != v.size(); ++i)
    {
        ASSERT_EQ(v.at(i).value, (v.begin() + i)->value);
        ASSERT_EQ(v.at(i).value, (*(v.begin() + i)).value);
        ASSERT_EQ(v.at(i).value, (*(v.end() + (i - v.size()))).value);
        ASSERT_EQ(v.at(i).value, (*(v.end() - (v.size() - i))).value);
    }
}
TEST_F(persistent_vector_test, iterators_should_be_equality_comparable)
{
    auto n = numbers(4 * 4 - 1);
    string_vector v{begin(n), end(n)};
    ASSERT_TRUE(v.begin() + v.size() == v.end());
    ASSERT_FALSE(v.begin() + v.size() != v.end());
    ASSERT_FALSE(v.begin() == v.end());
    ASSERT_TRUE(v.begin() != v.end());
}

TEST_F(persistent_vector_test, iterators_should_be_incrementable_and_decrementable)
{
    auto n = numbers(4 * 4 - 1);
    string_vector v{begin(n), end(n)};
    auto it = v.begin();
    ASSERT_TRUE(it++ == v.begin());
    ASSERT_TRUE(it == v.begin() + 1);
    ASSERT_TRUE(++it == v.begin() + 2);
    ASSERT_TRUE(it == v.begin() + 2);
    ASSERT_TRUE(it-- == v.begin() + 2);
    ASSERT_TRUE(it == v.begin() + 1);
    ASSERT_TRUE(--it == v.begin());
    ASSERT_TRUE(it == v.begin());
}

TEST_F(persistent_vector_test, pop_back_should_remove_one_element_from_the_end)
{
    auto n = numbers(4 * 4 * 4 * 4 * 4);
    auto v = string_vector{begin(n), end(n)};
    for (auto i = v.size(); i > 0; --i)
    {
        auto old_trace_count = trace_count;
        auto popped = v.pop_back();
        ASSERT_EQ(i - 1, popped.size());
        for (std::size_t k = 0; k < popped.size(); ++k)
            EXPECT_EQ(n.at(k).value, popped.at(k).value) << "index " << k;
        ASSERT_LT(trace_count - old_trace_count, 4);
        v = popped;
        EXPECT_EQ(old_trace_count - 1, trace_count) << "index " << i;
    }
}

}
