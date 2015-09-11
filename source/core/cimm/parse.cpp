#include "parse.hpp"
#include <boost/spirit/home/qi.hpp>

namespace cimm
{

namespace
{

namespace qi = ::boost::spirit::qi;
namespace ascii = ::boost::spirit::ascii;

template <typename iterator>
struct expression_grammar : boost::spirit::qi::grammar<iterator, expression_variant(), ascii::space_type>
{
    expression_grammar() : expression_grammar::base_type(grammar, "expression-grammar") { }

    template <typename type>
    using rule = qi::rule<iterator, type(), ascii::space_type>;

    rule<list> list_rule{qi::lit('(') >> qi::attr(list{}) >> qi::lit(')')};
    rule<expression_variant> grammar{qi::int_ | list_rule | *qi::char_};
};

}

auto parse_expression(const string& expr_text) -> expression
{
    auto first = begin(expr_text);
    expression_grammar<std::string::const_iterator> grammar;
    expression_variant expr;

    boost::spirit::qi::phrase_parse(first, end(expr_text), grammar, ascii::space, expr);

    return {expr};
}

}
