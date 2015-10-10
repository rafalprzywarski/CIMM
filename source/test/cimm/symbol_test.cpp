#include "eval_test.hpp"

namespace cimm
{

struct symbol_test : eval_test { };

TEST_F(symbol_test, should_make_symbols_from_strings)
{
    EXPECT_EQ(expression(symbol("a")), evaluate(list{symbol("symbol"), string("a")}));
}

TEST_F(symbol_test, should_fail_to_create_symbols_from_other_types)
{
    assert_evaluation_error<cannot_create_symbol>("cannot create a symbol from :abc", "(symbol :abc)");
    ASSERT_THROW(evaluate(list{symbol("symbol"), nil}), cannot_create_symbol);
    ASSERT_THROW(evaluate(list{symbol("symbol"), integer(5)}), cannot_create_symbol);
}

TEST_F(symbol_test, should_make_symbols_from_the_first_parameter_only)
{
    EXPECT_EQ(expression(symbol("a")), evaluate(list{symbol("symbol"), string("a"), string("b")}));
}

TEST_F(symbol_test, should_fail_for_no_parameters)
{
    assert_evaluation_error<cannot_create_symbol>("cannot create a symbol from nil", "(symbol)");
}

}
