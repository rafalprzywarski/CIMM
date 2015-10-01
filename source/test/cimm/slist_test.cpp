#include <gtest/gtest.h>
#include <cimm/expression.hpp>

namespace cimm
{

TEST(slist_test, should_be_empty_when_default_constructed)
{
    ASSERT_EQ(0, slist{}.count());
    ASSERT_EQ(nil, slist{}.first());
}

TEST(slist_test, should_be_constructible_from_one_element)
{
    slist l{expression(integer(5))};
    ASSERT_EQ(1, l.count());
    ASSERT_EQ(integer(5), l.first());
}

TEST(slist_test, should_be_constructible_from_an_element_and_a_list)
{
    slist empty{};

    slist l1{expression(integer(11)), empty};
    ASSERT_EQ(1, l1.count());
    ASSERT_EQ(integer(11), l1.first());
    ASSERT_EQ(0, l1.next().count());
    ASSERT_EQ(nil, l1.next().first());

    slist l2{expression(integer(22)), l1};
    ASSERT_EQ(2, l2.count());
    ASSERT_EQ(integer(22), l2.first());
    ASSERT_EQ(1, l2.next().count());
    ASSERT_EQ(integer(11), l2.next().first());
}

TEST(slist_test, cons_should_create_a_new_list_an_element_added_at_the_beginning)
{
    auto l = slist{}.cons(nil);
    ASSERT_EQ(1, l.count());
    ASSERT_EQ(nil, l.first());
    auto l2 = l.cons(integer(5));
    ASSERT_EQ(2, l2.count());
    ASSERT_EQ(integer(5), l2.first());
}

TEST(slist_test, next_should_return_an_empty_list_for_an_empty_list)
{
    auto l = slist{}.next();
    ASSERT_EQ(0, l.count());
    ASSERT_EQ(nil, l.first());
}

TEST(slist_test, next_should_return_a_list_without_the_first_element)
{
    auto l = slist{}.cons(integer(2)).cons(integer(3)).cons(integer(5)).next();
    ASSERT_EQ(2, l.count());
    ASSERT_EQ(integer(3), l.first());
    auto l2 = l.next();
    ASSERT_EQ(1, l2.count());
    ASSERT_EQ(integer(2), l2.first());
    auto l3 = l2.next();
    ASSERT_EQ(0, l3.count());
    ASSERT_EQ(nil, l3.first());
}

TEST(slist_test, should_be_equality_comparable)
{
    ASSERT_TRUE(slist{} == slist{});
    ASSERT_FALSE(slist{} == slist{integer(5)});
    ASSERT_FALSE(slist{integer(5)} == slist{});
    ASSERT_FALSE(slist{nil} == slist{});
    ASSERT_TRUE(slist{integer(5)} == slist{integer(5)});
    ASSERT_FALSE(slist{integer(7)} == slist{integer(5)});
    ASSERT_TRUE((slist{integer(5), slist{integer(7)}} == slist{integer(5), slist{integer(7)}}));
    ASSERT_FALSE((slist{integer(5)} == slist{integer(5), slist{integer(7)}}));
    ASSERT_FALSE((slist{integer(5), slist{integer(7)}} == slist{integer(5)}));
    ASSERT_FALSE((slist{integer(5), slist{integer(6)}} == slist{integer(5), slist{integer(7)}}));
    ASSERT_FALSE((slist{integer(5), slist{integer(7)}} == slist{integer(5), slist{integer(6)}}));
}

}
