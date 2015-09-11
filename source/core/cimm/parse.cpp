#include "parse.hpp"
#include <boost/spirit/home/qi.hpp>

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
        expression_variant_rule = qi::int_ | list_rule | string_rule;
    }

    template <typename type>
    using rule = qi::rule<iterator, type(), ascii::space_type>;

    rule<string> string_rule{qi::no_skip[+(qi::char_ - ')' - ' ')]};
    rule<expression_variant> expression_variant_rule;
    rule<expression> expression_rule{qi::as<expression>()[expression_variant_rule]};
    rule<std::vector<expression>> vector_rule{qi::lit('(') >> *expression_rule >> qi::lit(')')};
    rule<list> list_rule{qi::as<list>()[vector_rule]};
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
