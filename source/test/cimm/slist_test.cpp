#include <gtest/gtest.h>
#include <cimm/slist.hpp>

namespace cimm
{

TEST(slist_test, should_be_empty_when_default_constructed)
{
    ASSERT_EQ(0, slist{}.count());
    ASSERT_EQ(nil, slist{}.first());
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

}
