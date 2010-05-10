// oodles
#include "URL.hpp"

// STL
#include <sstream>

// STL
using std::copy;
using std::string;
using std::vector;
using std::ostream;
using std::ostringstream;
using std::ostream_iterator;

namespace oodles {
namespace url {

URL::URL(const string &url)
{
    tokenise(url);
}

string
URL::to_string() const
{
    ostringstream stream;

    stream << attributes.scheme << "://";

    if (!attributes.username.empty())
        stream << attributes.username << ':' << attributes.password << '@';

    if (!attributes.domain.empty()) {
        vector<value_type>::const_iterator i = attributes.domain.begin(),
                                           j = attributes.domain.end() - 1;
        copy(i, j, ostream_iterator<value_type>(stream, "."));
        stream << *j;
    }

    if (!attributes.port.empty() && attributes.port != "80")
        stream << ':' << attributes.port;

    if (!attributes.path.empty()) {
        vector<value_type>::const_iterator i = attributes.path.begin(),
                                           j = attributes.path.end() - 1;

        stream << '/';
        copy(i, j, ostream_iterator<value_type>(stream, "/"));
        stream << *j;
    }
    
    stream << '/' << attributes.page;

    return stream.str();
}

void
URL::print(ostream &stream) const
{
    stream << to_string();
}

bool
URL::operator==(const URL &rhs) const
{
    // TODO: Flesh me out by perhaps using the ID/hash of all components
    return true;
}

bool
URL::operator!=(const URL &rhs) const
{
    return !(operator==(rhs));
}

void
URL::tokenise(const string &url) throw(ParseError)
{
    Parser p;
    value_type::const_iterator i = url.begin(), j = url.end();

    if (!p.parse(i, j, attributes))
       throw ParseError("URL::tokenise", 0, "Failed to parse input '%s'.",
                        url.c_str());
}

} // url
} // oodles
