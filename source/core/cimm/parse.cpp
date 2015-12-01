#include "parse.hpp"
#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_expect.hpp>

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
    return list{special::quote, e};
}

template <typename iterator>
struct expression_grammar : boost::spirit::qi::grammar<iterator, std::vector<expression>(), ascii::space_type>
{
    expression_grammar() : expression_grammar::base_type(expressions_rule, "expression-grammar")
    {
        newline.add("\\n", '\n');
        quote_rule = qi::lit('\'') > expression_rule[_val = bind(quote_expr, _1)];
        expression_variant_rule = qi::int_ | boolean_rule | list_rule | vector_rule | string_rule | keyword_rule | nil_rule | symbol_rule;
    }

    template <typename type>
    using rule = qi::rule<iterator, type(), ascii::space_type>;

    qi::symbols<char const, char const> newline;
    rule<string> string_rule{qi::lit('\"') >> qi::no_skip[*(newline | (qi::char_ - '\"'))] >> qi::lit('\"')};
    decltype(qi::char_ - ')' - ']' - ' ') symbol_char{qi::char_ - ')' - ']' - ' '};
    rule<string> char_seq_rule{qi::no_skip[+symbol_char]};
    rule<string> keyword_char_seq_rule{qi::lit(':') >> char_seq_rule};
    rule<keyword> keyword_rule{keyword_char_seq_rule};
    rule<symbol> symbol_rule{char_seq_rule};
    rule<expression_variant> expression_variant_rule;
    rule<list> quote_rule;
    rule<expression> expression_rule{qi::as<expression>()[quote_rule | expression_variant_rule]};
    rule<std::vector<expression>> expressions_rule = *expression_rule;
    rule<std::vector<expression>> list_vector_rule{qi::lit('(') >> *expression_rule > qi::lit(')')};
    rule<std::vector<expression>> vector_vector_rule{qi::lit('[') >> *expression_rule > qi::lit(']')};
    rule<boolean> boolean_rule{(qi::no_skip[qi::lit("true") >> !symbol_char] >> qi::attr(true)) | (qi::no_skip[qi::lit("false") >> !symbol_char] >> qi::attr(false))};
    rule<nil_type> nil_rule{qi::no_skip[qi::lit("nil") >> !symbol_char] >> qi::attr(nil)};
    rule<list> list_rule{list_vector_rule};
    rule<vector> vector_rule{vector_vector_rule};
};

}

auto parse_expression(const string& expr_text) -> expression
{
    return first(parse_expressions(expr_text));
}

auto parse_expressions(const string& expr_text) -> vector
{
    auto first = begin(expr_text);
    expression_grammar<std::string::const_iterator> grammar;
    std::vector<expression> exprs;

    try
    {
        boost::spirit::qi::phrase_parse(first, end(expr_text), grammar, ascii::space, exprs);
    }
    catch (boost::spirit::qi::expectation_failure<std::string::const_iterator> const& e)
    {
        throw parse_error("unexpected EOF");
    }

    if (first != end(expr_text))
        throw parse_error(string{"unexpected "} + *first);

    return vector{exprs};
}

}
