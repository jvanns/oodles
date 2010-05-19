#ifndef OODLES_URL_PARSER_HPP
#define OODLES_URL_PARSER_HPP

// oodles
#include "type.hpp"

// Boost.spirit
#include <boost/spirit/include/qi.hpp>

namespace oodles {
namespace url {

namespace qi = boost::spirit::qi;

struct Attributes
{
    bool ip; // Is the domain stored in fact an IP address?

    value_type port,
               page,
               scheme,
               username,
               password;
    std::vector<value_type> path, domain;

    Attributes() : ip(false) {} // Assume not to begin with
};

struct Parser : qi::grammar<value_type::const_iterator, Attributes()>
{
    Parser(); // Definition of the grammar is constructed here
    
    bool parse(value_type::const_iterator &begin,
               value_type::const_iterator &end,
               Attributes &url);
    /*
     * There is no reason for these to be public really.
     */
    private:
        bool ip;
        qi::rule<value_type::const_iterator, Attributes()> url;
        qi::rule<value_type::const_iterator, value_type()> dir,
                                                           page,
                                                           port,
                                                           scheme,
                                                           username,
                                                           password;
        qi::rule<value_type::const_iterator, std::vector<value_type>()> path,
                                                                        domain;
};

} // url
} // oodles

#endif
