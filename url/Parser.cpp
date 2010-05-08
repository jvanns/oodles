// oodles
#include "Parser.hpp"

// Boost.spirit
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

// Boost.fusion
#include <boost/fusion/include/adapt_struct.hpp>

// STL
using std::vector;

BOOST_FUSION_ADAPT_STRUCT(
    oodles::url::Attributes,
    (oodles::url::value_type, port)
    (oodles::url::value_type, page)
    (oodles::url::value_type, scheme)
    (oodles::url::value_type, username)
    (oodles::url::value_type, password)
    (std::vector<oodles::url::value_type>, path)
    (std::vector<oodles::url::value_type>, domain)
)

/*
 * Functor for use with spirit:qi and phoenix::function
 * checks input of a path fragment first before appending
 * it to the container. 
 *
 * We wish to remove '.' and '..' components in an absolute
 * path.
 */
struct normalise_path_fragment_impl
{
    template<typename Container, typename Type>
    struct result
    {
        typedef void type;
    };

    template<typename Container, typename Type>
    void operator() (Container &c, const Type &v) const
    {
        static const Type cd("."), // Current Directory
                          pd(".."); // Previous Directory

        if (v != cd && v != pd)
            c.push_back(v);
        else if (v == pd && !c.empty())
            c.erase(c.end() - 1);
    }
};

boost::phoenix::function<normalise_path_fragment_impl> normalise_path_fragment;

namespace oodles {
namespace url {

/*
 * Use Spirit here - 'tis a simple syntax and we could scan it
 * sequentially efficiently enough but we'll compare results to
 * be sure...
 *
 * Format:
 * scheme://username:password@domain.com:port/path/page?query_string#anchor
 */
Parser::Parser() : Parser::base_type(url, "url")
{
    using boost::phoenix::at_c;
    using boost::phoenix::empty;

    using boost::spirit::qi::_1;
    using boost::spirit::qi::_val;

    using boost::spirit::qi::eoi; // End Of Input
    using boost::spirit::qi::lit; // Literal
    using boost::spirit::qi::omit;
    using boost::spirit::qi::no_case; // Ignore case
    using boost::spirit::ascii::char_;
    using boost::spirit::ascii::digit;

    /*
     * Scheme: All characters up to but not including the terminator '://'
     */
    scheme %= +(char_ - lit("://")) >> omit[lit("://")];

    /*
     * Username: All characters up to but not including the terminator ':'
     */
    username %= +(~char_(":.@")) >> omit[':'];

    /*
     * Password: All characters up to but not including the terminator '@'
     */
    password %= +(char_ - '@') >> omit['@'];

    /*
     * Domain: A period delimited sequence of strings up to a path or port
     */
    domain %=
        +(no_case[~char_(".:/")]) % '.'
        >> -omit[char_(':') | '/']
    ;

    /*
     * Port: Er, a port number (up to and certainly not including '/' ;)
     */
    port %= +(digit - '/') >> -omit['/'];

    /*
     * Path: A forward-slash delimited sequence of strings up to the page
     */
    dir %= +(char_ - '/'); // Identifies a single path fragment
    path = *(dir >> '/')[normalise_path_fragment(_val, _1)] >> -omit['/'];

    /*
     * Page: Leaf node of the path - the last of the URL (drop the #fragment)
     */
    page %= +(char_ - '#') >> -omit[lit('#') >> +(char_ -  eoi)] || eoi;

    url =
        -scheme[at_c<2>(_val) = _1] /* Scheme is not always present in links */
        >> -(username[at_c<3>(_val) = _1] >  /* Most domains have no username */
             password[at_c<4>(_val) = _1]) /* But those that do have password */
        >> -domain[at_c<6>(_val) = _1] /* Absolute URLs will have a domain */
        >> -port[at_c<0>(_val) = _1] /* And maybe a port number too */
        >> -path[at_c<5>(_val) = _1] /* And of course perhaps a file path */
        >> -page[at_c<1>(_val) = _1] /* Any further characters are the page */
    ;

    /* Give the rules an appropriate name */
    url.name("url");
    port.name("Port number");
    page.name("Page/document");
    scheme.name("Scheme/protocol");
    username.name("Domain username");
    password.name("Domain password");
    domain.name("Site domain components");
    path.name("Path/directory hierarchy");

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
        url,
        cerr << val("Error! Expecting ")
             << _4
             << val(" here: \"")
             << construct<value_type>(_3, _2)
             << val("\"")
             << endl
    );
}

bool
Parser::parse(value_type::const_iterator &begin,
              value_type::const_iterator &end,
              Attributes &url)
{
    if (qi::parse(begin, end, *this, url))
        if (begin == end)
            return true; // Parse was both successful and completed in full

    return false;
}

} // url
} // oodles
