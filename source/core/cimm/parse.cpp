#include "parse.hpp"
#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace cimm
{

namespace
{

namespace qi = ::boost::spirit::qi;
namespace ascii = ::boost::spirit::ascii;
using qi::labels::_val;
using qi::labels::_1;

auto quote_expr(const expression& e) -> list
{
    return list{quote, e};
}

template <typename iterator>
struct expression_grammar : boost::spirit::qi::grammar<iterator, expression(), ascii::space_type>
{
    expression_grammar() : expression_grammar::base_type(expression_rule, "expression-grammar")
    {
        quote_rule = qi::lit('\'') > expression_rule[_val = bind(quote_expr, _1)];
        expression_variant_rule = qi::int_ | boolean_rule | list_rule | string_rule | keyword_rule | symbol_rule;
    }

    template <typename type>
    using rule = qi::rule<iterator, type(), ascii::space_type>;

    rule<string> string_rule{qi::lit('\"') >> qi::no_skip[*(qi::char_ - '\"')] >> qi::lit('\"')};
    rule<string> char_seq_rule{qi::no_skip[+(qi::char_ - ')' - ' ')]};
    rule<string> keyword_char_seq_rule{qi::no_skip[qi::lit(':') >> +(qi::char_ - ')' - ' ')]};
    rule<keyword> keyword_rule{keyword_char_seq_rule};
    rule<symbol> symbol_rule{char_seq_rule};
    rule<expression_variant> expression_variant_rule;
    rule<list> quote_rule;
    rule<expression> expression_rule{qi::as<expression>()[quote_rule | expression_variant_rule]};
    rule<std::vector<expression>> vector_rule{qi::lit('(') >> *expression_rule >> qi::lit(')')};
    rule<boolean> boolean_rule{(qi::lit("true") >> qi::attr(true)) | (qi::lit("false") >> qi::attr(false))};
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
