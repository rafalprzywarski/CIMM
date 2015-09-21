#include "eval_test.hpp"

namespace cimm
{

struct def_test : eval_test { };

TEST_F(def_test, should_define_constants)
{
    EXPECT_EQ(nil, evaluate(list{special::def, symbol("seven"), integer(7)}));
    EXPECT_EQ(integer(7), evaluate(symbol("seven")));
    EXPECT_EQ(nil, evaluate(list{special::def, symbol("name"), string("Sonia")}));
    EXPECT_EQ(string("Sonia"), evaluate(symbol("name")));
}

TEST_F(def_test, should_fail_when_a_symbol_is_already_defined)
{
    evaluate(list{special::def, symbol("const1"), nil});
    try
    {
        evaluate(list{special::def, symbol("const1"), nil});
        FAIL() << "expected an exception";
    }
    catch (symbol_already_defined const& e)
    {
        ASSERT_STREQ("symbol 'const1' already defined", e.what());
    }
}

TEST_F(def_test, should_fail_when_trying_to_evaluate_an_undefined_symbol)
{
    try
    {
        evaluate(symbol("badbad"));
        FAIL() << "expected an exception";
    }
    catch (undefined_symbol_error const& e)
    {
        ASSERT_STREQ("undefined symbol 'badbad'", e.what());
    }
}

TEST_F(def_test, should_evaluate_the_given_expression)
{
    EXPECT_EQ(nil, evaluate(list{special::def, symbol("a_list"), list{special::quote, list{integer(7), integer(2)}}}));
    EXPECT_EQ((list{integer(7), integer(2)}), evaluate(symbol("a_list")));
}

TEST_F(def_test, should_not_define_a_symbol_if_the_evaluation_fails)
{
    try
    {
        evaluate(list{special::def, symbol("x"), symbol("u")});
    }
    catch (evaluation_error& ) { }
    ASSERT_NO_THROW(evaluate(list{special::def, symbol("x"), nil}));
}

TEST_F(def_test, should_alias_functions)
{
    evaluate(list{special::def, symbol("add"), symbol("+")});
    ASSERT_EQ(integer(5), evaluate(list{symbol("add"), integer(2), integer(3)}));
}

}
