#include "parse.hpp"
#include <boost/spirit/home/qi.hpp>
/*
namespace boost
{
namespace spirit
{
namespace traits
{

template <>
struct transform_attribute<cimm::string, cimm::identifier, qi::domain, void>
{
    typedef cimm::identifier type;
    static type pre(const cimm::string& val) { return type(); }
    static void post(cimm::identifier& val, type attr)
    {
        val = cimm::identifier{attr};
    }

    static void fail(cimm::identifier&) { }
};

}
}
}
*/
namespace cimm
{

namespace
{

namespace qi = ::boost::spirit::qi;
namespace ascii = ::boost::spirit::ascii;

template <typename iterator>
struct expression_grammar : boost::spirit::qi::grammar<iterator, expression(), ascii::space_type>
{
    expression_grammar() : expression_grammar::base_type(expression_rule, "expression-grammar")
    {
        expression_variant_rule = qi::int_ | list_rule | identifier_rule;
    }

    template <typename type>
    using rule = qi::rule<iterator, type(), ascii::space_type>;

    rule<string> string_rule{qi::no_skip[+(qi::char_ - ')' - ' ')]};
    rule<identifier> identifier_rule{qi::lexeme[+(qi::char_ - ')' - ' ')]};
    rule<expression_variant> expression_variant_rule;
    rule<expression> expression_rule{qi::as<expression>()[expression_variant_rule]};
    rule<std::vector<expression>> vector_rule{qi::lit('(') >> *expression_rule >> qi::lit(')')};
    rule<list> list_rule{vector_rule};
};

}

auto parse_expression(const string& expr_text) -> expression
{
    auto first = begin(expr_text);
    expression_grammar<std::string::const_iterator> grammar;
    expression expr;

    boost::spirit::qi::phrase_parse(first, end(expr_text), grammar, ascii::space, expr);

    return expr;
}

}
