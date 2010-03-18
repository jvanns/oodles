#ifndef OODLES_URL_HPP
#define OODLES_URL_HPP

// oodles
#include "Iterator.hpp"
#include "TreeIterator.hpp"

// STL
#include <set>
#include <string>
#include <vector>
#include <iostream>

namespace oodles {
namespace url {

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
        void print(std::ostream &stream) const;

        Iterator begin() const
        {
            return Iterator(this, Iterator::Begin);
        }
        
        Iterator end() const
        {
            return Iterator(this, Iterator::End);
        }
        
        TreeIterator begin_tree() const
        {
            return TreeIterator(this, Iterator::Begin);
        }
        
        TreeIterator end_tree() const
        {
            return TreeIterator(this, Iterator::End);
        }
    public:
        typedef Iterator iterator;
        typedef TreeIterator tree_iterator;

        typedef std::string value_type;
        typedef std::pair<value_type, value_type> query_kvp;

        enum {Scheme, Username, Password, Domain, Port, Path, Page, Query};
    private:
        void normalise();
        void tokenise(const std::string &url);
    private:
        friend class Iterator; // We need to give iterators access to...
        friend class TreeIterator; // We need to give iterators access to...
    private:
        value_type port,
                   page, // i.e. index.html
                   scheme, // i.e. http (ignoring ://)
                   username,
                   password;
        std::vector<value_type> path, // i.e. /product, /ids etc.
                                domain; // i.e. www, google, co, uk
        std::set<query_kvp> query_string; // i.e. name=Jim&uuid=12345
};

static
inline
std::ostream&
operator<< (std::ostream &stream, const URL &url)
{
    url.print(stream);
    return stream;
}

} // url
} // oodles

#endif
