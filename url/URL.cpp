// oodles
#include "URL.hpp"

// STL
#include <sstream>

// libc
#include <assert.h> // For assert()

// STL
using std::copy;
using std::string;
using std::vector;
using std::ostream;
using std::ostringstream;
using std::ostream_iterator;

namespace oodles {
namespace url {

URL::URL(const string &url) :
    domain(attributes.domain),
    path(attributes.path),
    page(attributes.page),
    url_id(tokenise(url))
{
    assert(url_id == page_id());
}

URL::hash_t
URL::page_id() const
{
    return page.id();
}

URL::hash_t
URL::path_id() const
{
    return path.id();
}

URL::hash_t
URL::domain_id() const
{
    return domain.id();
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
URL::operator==(URL &rhs) const
{
    return url_id == rhs.url_id;
}

bool
URL::operator!=(URL &rhs) const
{
    return !(operator==(rhs));
}

URL::hash_t
URL::tokenise(const string &url) throw(ParseError)
{
    Parser p;
    value_type::const_iterator i = url.begin(), j = url.end();

    if (!p.parse(i, j, attributes))
       throw ParseError("URL::tokenise", 0, "Failed to parse input '%s'.",
                        url.c_str());
    
    return page.id(path.id(domain.id(0)));
}

} // url
} // oodles
