// oodles
#include "URL.hpp"

// STL
#include <sstream>

// STL
using std::map;
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
    int state = oodles::URL::Scheme;

    if (url[index] == ':') { // Predict next state
        index += 2; // Skip over ':/'

        if (url.find_last_of('@', url.find_first_of('/', index + 1)) != NONE)
            state = oodles::URL::Username;
        else
            state = oodles::URL::Domain;
    } else {
        scheme += url[index];
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
    int state = oodles::URL::Domain;
    string::size_type i = 0, j = 0, max = url.size();

    if ((j = url.find_first_of('/', index)) == NONE)
        j = max; // No path or page given, just a domain

    if ((i = url.find_last_of(':', j)) > index)
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
            *s += url[index];
    }

    if (url[index] == ':')
        state = oodles::URL::Port;
    else if (url[index] == '/')
        state = oodles::URL::Path;
    else
        state = oodles::URL::Page;

    return state;
}

static
inline
int
tokenise_path(const string &url,
              string::size_type &index,
              vector<string> &path)
{
    int state = oodles::URL::Page;
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
               map<string, string> &query_string)
{
    int state = oodles::URL::Query;
    string::size_type i = 0, j = 0, max = url.size();

    if ((i = url.find_first_of('#', index)) != NONE) // Skip any anchor
        max = i - 1;

    while (index < max) {
        i = url.find_first_of('=', index);
        if ((j = url.find_first_of('&', i)) == NONE)
            j = max + 1;

        query_string[url.substr(index, j - i)] =
                     url.substr(i + 1, (j - 1) - i);
        index = j + 1;
    }

    return state;
}

namespace oodles {

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

    if (!path.empty()) {
        copy(path.begin(),
             path.end() - 1,
             ostream_iterator<value_type>(stream, "/"));
        stream << *(path.end() - 1);
    }

    stream << page;

    if (!query_string.empty()) {
        map<value_type, value_type>::const_iterator i = query_string.begin();

        stream << '?';
        while (i != query_string.end()) {
            stream << i->first << '=' << i->second << '&';
            ++i;
        }
    }

    return stream.str();
}

ostream&
URL::print(ostream &stream) const
{
    return stream << to_string();
}

bool
URL::operator==(const URL &rhs) const
{
    return false;
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
        value_type &root = path.front(), &leaf = path.back();
        root.insert(root.begin(), '/');
        leaf += '/';

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
    } else if (!page.empty()) {
        page.insert(page.begin(), '/');
    }

    if (!port.empty() && port != "80") {
        value_type &dc = domain.back();
        dc += ":" + port;
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
    string::size_type i = 0, max = url.size();

    for (int s = Scheme ; i < max ; ++i) {
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

} // oodles
