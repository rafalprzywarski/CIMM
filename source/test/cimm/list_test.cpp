#include "eval_test.hpp"

namespace cimm
{

struct list_test : eval_test { };

TEST_F(list_test, should_create_a_list)
{
    EXPECT_EQ(expression(list{}), evaluate(list{symbol("list")}));
    EXPECT_EQ(expression(list{integer(5)}), evaluate(list{symbol("list"), integer(5)}));
    EXPECT_EQ(expression(list{keyword("a"), string("b")}), evaluate(list{symbol("list"), keyword("a"), string("b")}));
}

}
