// oodles
#include "URL.hpp"

// STL
#include <sstream>

// STL
using std::set;
using std::pair;
using std::copy;
using std::string;
using std::vector;
using std::ostream;
using std::ostringstream;
using std::ostream_iterator;

static const string::size_type NONE = string::npos;

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
        index += 2; // Skip over ':/'

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
                vector<string> &domain)
{
    int state = oodles::url::URL::Domain;
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
        if (url[index] == '.')
            s = &domain[++i]; // Reference next domain component
        else
            *s += tolower(url[index]); // Normalise as we go :)
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
               set<oodles::url::URL::query_kvp> &query_string)
{
    int state = oodles::url::URL::Query;
    string::size_type i = 0, j = 0, max = url.size();

    if ((i = url.find_first_of('#', index)) != NONE) // Skip any anchor
        max = i - 1;

    while (index < max) {
        i = url.find_first_of('=', index);
        if ((j = url.find_first_of('&', i)) == NONE)
            j = max + 1;

        const oodles::url::URL::query_kvp p(url.substr(index, i - index),
                                            url.substr(i + 1, (j - 1) - i));

        query_string.insert(p);
        index = j + 1;
    }

    return state;
}

namespace oodles {
namespace url {

URL::URL(const string &url)
{
    tokenise(url);
    normalise();
}

string
URL::to_string() const
{
    ostringstream stream;

    stream << scheme << "://";

    if (!username.empty())
        stream << username << ':' << password << '@';

    copy(domain.begin(),
         domain.end() - 1,
         ostream_iterator<value_type>(stream, "."));
    stream << *(domain.end() - 1);

    if (!port.empty() && port != "80")
        stream << ':' << port;

    if (!path.empty()) {
        vector<value_type>::const_iterator i = path.begin(), j = path.end() - 1;

        stream << '/';
        copy(i, j, ostream_iterator<value_type>(stream, "/"));
        stream << *j << '/';
    } else
        stream << '/';

    stream << page;

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
    if (page != rhs.page)
        return false;

    if (path.size() != rhs.path.size())
        return false;

    if (domain.size() != rhs.domain.size())
        return false;

    /* TODO: Add the more complex comparison tests if we get this far */
    return true;
}

bool
URL::operator!=(const URL &rhs) const
{
    return !(operator==(rhs));
}

void
URL::normalise()
{
    if (!path.empty()) {
        vector<value_type>::iterator i = path.begin(), j = path.end(), k;
        while (i != j) {
            if (*i == "..") {
                k = i;

                if ((k + 1) != j)
                    ++k;

                path.erase(i);
                i = k;
            } else if (*i == ".") {
                k = i;

                if (k != path.begin())
                    --k;

                path.erase(i);
                i = k;
            }

            ++i;
        }
    }

    if (!query_string.empty()) {
        set<query_kvp>::iterator i = query_string.begin(),
                                 j = query_string.end();
        page += "?";

        while (i != j) {
            page += i->first + "=" + i->second + "&";
            ++i;
        }

        page.erase(page.end() - 1); // Remove trailing '&'
        query_string.clear();
    }
}

/*
 * No need for Spirit here - 'tis a simple syntax and we can scan it
 * sequentially efficiently enough. Follow the rule:
 *
 * scheme://username:password@domain.com:port/path/page?query_string#anchor
 */
void
URL::tokenise(const string &url)
{
    int s = Scheme; // Normally a URL will start with a scheme such as http...
    string::size_type i = 0, max = url.size();

    if (url.find_first_of("://") > url.find_first_of('.'))
        s = Domain; // ...however, some links begin with only a domain

    for ( ; i < max ; ++i) {
        switch (s) {
            case Scheme:
                s = tokenise_scheme(url, i, scheme);
                break;
            case Username:
                if (url[i] == ':')
                    s = Password;
                else
                    username += url[i];
                break;
            case Password:
                if (url[i] == '@')
                    s = Domain;
                else
                    password += url[i];
                break;
            case Domain:
                s = tokenise_domain(url, i, domain);
                break;
            case Port:
                if (url[i] == '/')
                    s = Path;
                else
                    port += url[i];
                break;
            case Path:
                s = tokenise_path(url, i, path);
                break;
            case Page:
                if (url[i] == '?')
                    s = Query;
                else
                    page += url[i]; // May in fact be a leaf of the path?
                break;
            case Query:
                s = tokenise_query(url, i, query_string);
                break;
        }
    }
}

} // url
} // oodles
