#include <gtest/gtest.h>
#include <cimm/slist.hpp>

namespace cimm
{

TEST(slist_test, should_be_empty_when_default_constructed)
{
    ASSERT_EQ(0, slist{}.count());
    ASSERT_EQ(nil, slist{}.first());
}

}
