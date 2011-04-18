// oodles
#include "URL.hpp"
#include "Parser.hpp"

// STL
#include <set>
#include <limits>

// libc
#include <stdlib.h> // For strtol()
#include <assert.h> // For assert()

// STL
using std::set;
using std::pair;
using std::string;
using std::vector;
using std::exception;
using std::numeric_limits;

static const string::size_type NONE = string::npos;

static
inline
int
str2int(const string &s)
{
    char *end;
    long result = 0;
    const char *str = s.c_str();
    static const long min = numeric_limits<long>::min(),
                      max = numeric_limits<long>::max();

    result = strtol(str, &end, 10);

    if (result == min && errno == ERANGE)
        throw oodles::TypeError("str2int",
                                errno,
                                "Integer underflow converting '%s'.", str);

    if (result == max && errno == ERANGE)
        throw oodles::TypeError("str2int",
                                errno,
                                "Integer overflow converting '%s'.", str);

    if (end == str || (end && *end != '\0'))
        throw oodles::TypeError("str2int",
                                errno,
                                "Cannot convert '%s' to integer.", str);

    return static_cast<int> (result);
}

static
inline
size_t
count_chars(const string &s, size_t start, size_t end, const char c = '.')
{
    size_t n = 0;

    while (start <= end)
        if (s[start++] == c)
            ++n;

    return n;
}

static
inline
int
tokenise_scheme(const string &url, string::size_type &index, string &scheme)
{
    int state = oodles::url::URL::Scheme;

    if (url[index] == ':') { // Predict next state
        index += 2; // Skip over '//'

        if (url.find_last_of('@', url.find_first_of('/', index + 1)) != NONE)
            state = oodles::url::URL::Username;
        else
            state = oodles::url::URL::Domain;
    } else {
        scheme += tolower(url[index]); // Normalise as we go :)
    }

    return state;
}

static
inline
int
tokenise_domain(const string &url,
                string::size_type &index,
                oodles::url::Attributes &attributes)
{
    uint16_t octets = 0;
    int state = oodles::url::URL::Domain;
    vector<string> &domain = attributes.domain;
    string::size_type i = 0, j = 0, max = url.size();

    if ((j = url.find_first_of('/', index)) == NONE)
        j = max; // No path or page given, just a domain

    if ((i = url.find_last_of(':', j)) != NONE && i > index)
        j = i; // We have a port to parse to

    i = 0; // Reset to use as index to domain

    /*
     * Reserve (pre-allocate) the correct no. of entries for our domain. The
     * no. domain components will be 1 more than the no. of dots delimiting it.
     */
    domain.resize(count_chars(url, index, j, '.') + 1);

    for (string *s = &domain[i] ; index < j ; ++index) {
        if (url[index] == '.') {
            try {
                str2int(*s);
                octets++;
            } catch (const exception &e) {
            }
            
            s = &domain[++i]; // Reference next domain component
        } else
            *s += tolower(url[index]); // Normalise as we go :)
    }

    try {
        str2int(domain[i]);
        attributes.ip = octets == 3 ? true : false;
    } catch (const exception &e) {
    }

    if (url[index] == ':')
        state = oodles::url::URL::Port;
    else if (url[index] == '/')
        state = oodles::url::URL::Path;
    else
        state = oodles::url::URL::Page;

    return state;
}

static
inline
int
tokenise_path(const string &url,
              string::size_type &index,
              vector<string> &path)
{
    int state = oodles::url::URL::Page;
    string::size_type i = 0, j = url.find_last_of('/');

    if (j == NONE || j == index - 1) {
        index--; // Will re-skip the single '/' that got us here
        return state; // Seems there are no further path fragments
    }

    /*
     * Reserve (pre-allocate) the correct no. of entries for our path. The
     * no. path components will be equal to the no. of slashes delimiting it.
     */
    path.resize(count_chars(url, index, j, '/')); // Reserve capacity

    for (string *s = &path[i] ; index < j ; ++index) {
        if (url[index] == '/')
            s = &path[++i]; // Reference next path component
        else
            *s += url[index];
    }

    return state; // Next (potential) state already set above
}

static
inline
int
tokenise_query(const string &url,
               string::size_type &index,
               set<pair<string, string> > *&query_string)
{
    int state = oodles::url::URL::Query;
    string::size_type i = 0, j = 0, max = url.size();

    if ((i = url.find_first_of('#', index)) != NONE) // Skip any anchor
        max = i - 1;

    assert(!query_string);
    query_string = new set<pair<string, string> >();
    
    while (index < max) {
        i = url.find_first_of('=', index);
        if ((j = url.find_first_of('&', i)) == NONE)
            j = max + 1;

        const pair<string, string> p(url.substr(index, i - index),
                                     url.substr(i + 1, (j - 1) - i));

        query_string->insert(p);
        index = j + 1;
    }

    return state;
}

static
inline
void
normalise(string &page,
          vector<string> &path,
          set<pair<string, string> > *&query_string)
{
    if (!path.empty()) {
        vector<string>::iterator i(path.begin()), j(path.end()), k;

        while (i != j) {
            k = i;
            
            if (*i == "..") {
                k -= 2;
                path.erase(i);
                path.erase(k + 1);
            } else if (*i == ".") {
                if (k != path.begin())
                    --k;

                path.erase(i);
            }

            i = k;
            ++i;
        }
    }

    if (query_string) {
        set<pair<string, string> >::iterator i(query_string->begin()),
                                             j(query_string->end());
        page += "?";

        while (i != j) {
            page += i->first + "=" + i->second + "&";
            ++i;
        }

        page.erase(page.end() - 1); // Remove trailing '&'
    }
}

namespace oodles {
namespace url {

/*
 * No need for Spirit here - 'tis a simple syntax and we can scan it
 * sequentially efficiently enough. Follow the rule:
 *
 * scheme://username:password@domain.com:port/path/page?query_string#anchor
 */
bool
Parser::parse(const string &url, Attributes &attributes)
{
    int s = URL::Scheme; // Normally a URL begins with a scheme such as http...
    string::size_type i = 0, max = url.size();
    set<pair<string, string> > *query_string = NULL;

    if (url.find_first_of("://") > url.find_first_of('.'))
        s = URL::Domain; // ...however, some links begin with only a domain

    for ( ; i < max ; ++i) {
        switch (s) {
            case URL::Scheme:
                s = tokenise_scheme(url, i, attributes.scheme);
                break;
            case URL::Username:
                if (url[i] == ':')
                    s = URL::Password;
                else
                    attributes.username += url[i];
                break;
            case URL::Password:
                if (url[i] == '@')
                    s = URL::Domain;
                else
                    attributes.password += url[i];
                break;
            case URL::Domain:
                s = tokenise_domain(url, i, attributes);
                break;
            case URL::Port:
                if (url[i] == '/')
                    s = URL::Path;
                else
                    attributes.port += url[i];
                break;
            case URL::Path:
                s = tokenise_path(url, i, attributes.path);
                break;
            case URL::Page:
                if (url[i] == '?')
                    s = URL::Query;
                else
                    attributes.page += url[i]; // May be a leaf of the path?
                break;
            case URL::Query:
                s = tokenise_query(url, i, query_string);
                break;
        }
    }

    normalise(attributes.page, attributes.path, query_string);
    delete query_string;

    return true;
}

} // url
} // oodles

