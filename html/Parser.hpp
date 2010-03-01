#ifndef OODLES_HTML_PARSER_HPP // Interface
#define OODLES_HTML_PARSER_HPP

// oodles
#include "document.hpp" // Our HTML document structures

// Boost.spirit
#include <boost/spirit/include/qi.hpp>

namespace oodles {
namespace html {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

typedef std::pair<std::string, std::string> Attribute;

// The declaration of our HTML grammar
template <class Iterator>
struct Parser : qi::grammar<Iterator,
                            Element(),
                            ascii::space_type>
{
    Parser(); // Definition of the grammar is constructed here

    const Element& document() const;
    bool parse(Iterator &begin, Iterator &end);

    /*
     * There is no reason for these to be public really.
     */
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

#include "Parser.ipp" // Implemention

#endif
