#ifndef OODLES_HTML_PARSER_H
#define OODLES_HTML_PARSER_H

// oodles
#include "document.hpp" // Our HTML document structures

// Boost.spirit
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

// Boost.fusion
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

// STL
#include <iostream>

namespace oodles {
namespace html {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

typedef std::pair<std::string, std::string> Attribute;

// The definition of our HTML grammar
template <class Iterator>
struct Parser : qi::grammar<Iterator,
                            Element(),
                            ascii::space_type>
{
    Parser() : Parser::base_type(html, "element")
    {
        using qi::_1;
        using qi::_val;

        using qi::lit;
        using qi::lexeme;
        using qi::matches;

        using ascii::char_;
        using ascii::space;

        /* Define all the parsers */

        /*
         * A tag name: All characters after '<' (but not "</")
         * and before any whitespace or '>'
         */
        tag_name
            %= '<'
            >> !lit('/')
            > lexeme[+(char_ -(space | '>'))];
        ;

        /*
         * Tag attribute: A key/value pair where key is all non-whitespace
         * characters up to an assignment ('=') and the value is all characters
         * after the assignment and an opening double quote but before the
         * closing quote
         */
        attribute
            %= +(char_ - '=')
            >> "=\""
            > lexeme[+(char_ - '"')] // Do not skip whitespace
            > '"'
        ;

        /*
         * A full tag definition: The name, 0 or more attributes
         * and a closing angled bracket.
         */
        tag
            %= tag_name
            > *(attribute - lit('>'))
            > '>'
        ;

        /*
         * Free-form text: Essentially the body or payload of a given node
         */
        text %= !lit("</") >> lexeme[+(char_ - '<')]; // Do not skip whitespace

        /*
         * The HTML document: A tag, 0 or more nested nodes (maybe text, maybe
         * another html node) and then an optional closing tag. The recursive
         * nature of HTML can be seen here (child rules<> of html).
         */
        html
            %= tag
            > *(text | html)
            > matches[(lit("</") > +(char_ - '>') > '>')]
        ;

        /* Give the rules an appropriate name */
        text.name("Text");
        html.name("Element"); // This rule is essentially 0 or more elements
        
        tag.name("Tag");
        tag_name.name("Tag name");
        attribute.name("Tag attribute");
        
        /* Install an error handler */
        using std::cerr;
        using std::endl;

        using qi::_2;
        using qi::_3;
        using qi::_4;

        using qi::fail;
        using qi::on_error;

        using boost::phoenix::val;
        using boost::phoenix::construct;

        on_error<fail>
        (
            html,
            cerr << val("Error! Expecting ")
                 << _4
                 << val(" here: \"")
                 << construct<std::string>(_3, _2)
                 << val("\"")
                 << endl
        );
    }

    bool parse(Iterator &begin, Iterator &end)
    {
        if (qi::phrase_parse(begin, end, *this, ascii::space, ast))
            if (begin == end) // Great. We parsed the whole document :)
                return true;

        return false;
    }

    const Element& document() const { return ast; } // Return the AST

    private:
        Element ast; // Our resulting Abstract Syntax Tree

        /*
         * The two rules for a Node (remember a Node may be
         * either another HTML Element or simply some Text.
         */
        qi::rule<Iterator, Text(), ascii::space_type> text;
        qi::rule<Iterator, Element(), ascii::space_type> html;
        
        /*
         * The rule for creating and storing a tag and
         * two rules for temporarily building the tag.
         */
        qi::rule<Iterator, Tag(), ascii::space_type> tag;
        qi::rule<Iterator, Tag::Name(), ascii::space_type> tag_name;
        qi::rule<Iterator, Attribute(), ascii::space_type> attribute;
};

} // html
} // oodles

// Tell fusion about our html document structures
BOOST_FUSION_ADAPT_STRUCT(
    oodles::html::Tag,
    (oodles::html::Tag::Name, name)
    (oodles::html::Tag::Attributes, attributes)
)

BOOST_FUSION_ADAPT_STRUCT(
    oodles::html::Element,
    (oodles::html::Tag, tag)
    (oodles::html::Nodes, children)
    (bool, terminated) // Some HTML elements are not :(
)

#endif
