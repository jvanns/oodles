#ifndef OODLES_URL_URL_HPP
#define OODLES_URL_URL_HPP

// oodles
#include "Parser.hpp"
#include "Iterator.hpp"
#include "TreeIterator.hpp"
#include "common/Exceptions.hpp"

// STL
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
        /* Dependent typedefs */
        typedef Iterator iterator;
        typedef TreeIterator tree_iterator;

        enum {Scheme, Username, Password, Domain, Port, Path, Page};

        /* Member functions/methods */
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
    private:
        /* Member functions/methods */
        void tokenise(const std::string &url) throw(ParseError);

        /* Member variables/attributes */
        Attributes attributes;

        /* Friend class declarations */
        friend class Iterator; // We need to give iterators access to...
        friend class TreeIterator; // We need to give iterators access to...
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
