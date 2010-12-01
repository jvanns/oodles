// oodles
#include "URL.hpp"
#include "utility/hash.hpp"

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

/*
 * Here, the anonymous namespace holds only the old IDGenerator template
 * class. Instead of being a class member however, or even instances of it,
 * we use it local to this unit only and return the hash_t values to an
 * instance of a simpler class, ID.
 */
namespace {

typedef oodles::url::URL::hash_t hash_t;
typedef oodles::url::value_type value_type;

// Define a local-only template function, hasher
template<class Container>
hash_t
hasher(const Container &input, hash_t seed);

// Provide the two necessary specialisations of hasher:
template<>
inline
hash_t
hasher<value_type>(const value_type &input, hash_t seed)
{
#ifdef HAS_64_BITS
    return oodles::fnv64(input.data(), input.size(), seed);
#else
    return oodles::fnv32(input.data(), input.size(), seed);
#endif
}

template<>
inline
hash_t
hasher<vector<value_type> >(const vector<value_type> &input, hash_t seed)
{
    vector<value_type>::const_iterator i = input.begin();

    for ( ; i != input.end() ; ++i)
        seed = hasher(*i, seed);

    return seed;
}

template<class Type>
class IDGenerator
{
    public:
        /* Member functions/methods */
        IDGenerator(const Type &value) : hash(0), content(value) {}
        hash_t id(hash_t seed = 0) { return compute_hash(seed); }
    private:
        /* Member functions/methods */
        hash_t compute_hash(hash_t seed)
        {
            if (!hash)
                hash = hasher(content, seed);

            return hash;
        }

        /* Member variables/attributes */
        hash_t hash;
        const Type &content;
};

} // anonymous

namespace oodles {
namespace url {

URL::URL(const string &url) : id(tokenise(url))
{
}

URL::URL(const URL &url) : id(url.id), attributes(url.attributes)
{
}

URL&
URL::operator= (const URL &url)
{
    if (this != &url) {
        id = url.id;
        attributes = url.attributes;
    }

    return *this;
}

bool
URL::operator==(URL &rhs) const
{
    return page_id() == rhs.page_id();
}

bool
URL::operator!=(URL &rhs) const
{
    return !(operator==(rhs));
}

URL::hash_t
URL::page_id() const
{
    return id.page;
}

URL::hash_t
URL::path_id() const
{
    return id.path;
}

URL::hash_t
URL::domain_id() const
{
    return id.domain;
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

URL::ID
URL::tokenise(const string &url) throw(ParseError)
{
    Parser p;
    IDGenerator<value_type> page(attributes.page);
    IDGenerator<vector<value_type> > path(attributes.path),
                                     domain(attributes.domain);
    value_type::const_iterator i = url.begin(),  j = url.end();

    if (!p.parse(i, j, attributes))
       throw ParseError("URL::tokenise", 0, "Failed to parse input '%s'.",
                        url.c_str());

    const ID x = {domain.id(),
                  path.id(domain.id()),
                  page.id(path.id(domain.id()))};
    return x;
}

} // url
} // oodles
