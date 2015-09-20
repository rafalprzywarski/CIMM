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

}
