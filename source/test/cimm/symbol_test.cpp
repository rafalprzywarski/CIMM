#include "eval_test.hpp"

namespace cimm
{

struct symbol_test : eval_test { };

TEST_F(symbol_test, should_make_symbols_from_stringlike_expressions)
{
    EXPECT_EQ(expression(symbol("a")), evaluate(list{symbol("symbol"), string("a")}));
    EXPECT_EQ(expression(symbol("a")), evaluate(list{symbol("symbol"), symbol("a")}));
    EXPECT_EQ(expression(symbol("a")), evaluate(list{symbol("symbol"), keyword("a")}));
    EXPECT_EQ(nil, evaluate(list{symbol("symbol"), nil}));
    EXPECT_EQ(nil, evaluate(list{symbol("symbol"), integer(5)}));
}

TEST_F(symbol_test, should_make_symbols_from_the_first_parameter_only)
{
    EXPECT_EQ(expression(symbol("a")), evaluate(list{symbol("symbol"), string("a"), string("b")}));
}

TEST_F(symbol_test, should_return_nil_for_no_parameters)
{
    EXPECT_EQ(expression(nil), evaluate(list{symbol("symbol")}));
}

}
