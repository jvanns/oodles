#ifndef OODLES_URL_HPP
#define OODLES_URL_HPP

// oodles
#include "Iterator.hpp"

// STL
#include <map>
#include <string>
#include <vector>
#include <iostream>

namespace oodles {

/*
 * Class to confom to URL generic syntax such as:
 * scheme://username:password@domain.com:port/path/page?query_string#anchor
 *
 * However, we discard the anchor suffix as it is unused anywhere in oodles!
 */
class URL
{
    public:
        URL(const std::string &url);

        bool operator== (const URL &rhs) const;
        bool operator!= (const URL &rhs) const;

        std::string to_string() const;
        std::ostream& print(std::ostream &stream) const;

        Iterator begin() { return Iterator(this, Iterator::Begin); }
        Iterator end() { return Iterator(this, Iterator::End); }
    public:
        typedef Iterator iterator;
        typedef std::string value_type;

        enum {Scheme, Username, Password, Domain, Port, Path, Page, Query};
    private:
        void normalise();
        void tokenise(const std::string &url);
    private:
        friend class Iterator; // We need to give iterators access to...
    private:
        value_type port,
                   page, // i.e. index.html
                   scheme, // i.e. http (ignoring ://)
                   username,
                   password;
        std::vector<value_type> path, // i.e. /product, /ids etc.
                                domain; // i.e. www, google, co, uk
        std::map<value_type, value_type> query_string; // i.e. name=Jim&a=1
};

static inline std::ostream& operator<< (std::ostream &stream, const URL &url)
{
    return url.print(stream);
}

} // oodles

#endif
