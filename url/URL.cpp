// oodles
#include "URL.hpp"

// STL
using std::map;
using std::string;
using std::vector;

namespace oodles {

URL::URL(const string &url) : port("80"), scheme("http")
{
    tokenise(url);
    normalise();
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
    string fragment;
    string::size_type i = 0, j = url.size(), k = string::npos, n = 0;
    enum state { Scheme, Username, Password, Domain, Port, Path, Page, Query };

    for (uint16_t s = Scheme ; i < j ; ++i) {
        switch (s) {
            case Scheme:
                if (url[i] == ':') { // Predict next state
                    i += 2; // Skip over ':/'

                    if (url.rfind('@', url.find_first_of('/', i + 1)) != k)
                        s = Username;
                    else
                        s = Domain;
                } else {
                    scheme += url[i];
                }
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
                if ((n = url.rfind(':', url.find_first_of('/', i))) == k)
                    if ((n = url.find_first_of('/', i)) == k)
                        n = j; // No path or page given, just a domain

                for (fragment.clear() ; i < n ; ++i) {
                    if (url[i] == '.') { // Store domain component
                        domain.push_back(fragment);
                        fragment.clear();
                    } else {
                        fragment += url[i];
                    }
                }

                domain.push_back(fragment);

                if (url[i] == ':')
                    s = Port;
                else if (url[i] == '/')
                    s = Path;
                break;
            case Port:
                if ((n = url.find_first_of('/', i)) == k)
                    n = j; // :port *may* be our last token

                port = url.substr(i, n);
                break;
            case Path:
                if ((n = url.find_last_of('/', i)) != k) {
                    for (fragment.clear() ; i < n ; ++i) {
                        if (url[i] == '/') { // Store path component
                            path.push_back(fragment);
                            fragment.clear();
                        } else {
                            fragment += url[i];
                        }
                    }

                    path.push_back(fragment);
                }
                s = Page; // FIXME: May in fact be a leaf of the path?
                break;
            case Page:
                if ((n = url.find_first_of('?', i)) == k)
                    n = j; // No query string found

                for ( ; i < n ; ++i)
                    page += url[i];

                if (j > n)
                    s = Query;
                break;
            case Query:
                if ((n = url.find_first_of('#', i)) != k) // Skip any anchor
                    j = n - 1;

                // TODO: Prase query string into k=v pairs
                break;
        }
    }
}

} // oodles
