#ifndef OODLES_URL_URL_HPP
#define OODLES_URL_URL_HPP

// oodles
#include "Parser.hpp"
#include "Iterator.hpp"
#include "TreeIterator.hpp"

#include "net/oop/Accessor.hpp"
#include "common/Exceptions.hpp"

// STL
#include <iostream>

// Test for 64-bit capability
#ifdef __GNUG__
#ifdef __LP64__
#define HAS_64_BITS
#endif
#endif

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
#ifdef HAS_64_BITS
        typedef uint64_t hash_t;
#else
        typedef uint32_t hash_t;
#endif
        typedef Iterator iterator;
        typedef TreeIterator tree_iterator;

        enum {Scheme, Username, Password, Domain, Port, Path, Page};

        /* Member functions/methods */
        URL();
        URL(const std::string &url);
        
        URL(const URL &url);
        URL& operator= (const URL &url);

        bool operator== (const URL &rhs) const;
        bool operator!= (const URL &rhs) const;

        hash_t page_id() const;
        hash_t path_id() const;
        hash_t domain_id() const;
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
        /* Internal structures */
        struct ID {
            hash_t domain, path, page;
        };

        /* Member functions/methods */
        ID tokenise(const std::string &url) throw(ParseError);

        /* Member variables/attributes */
        Attributes attributes;
        ID id;

        /* Friend class declarations */
        friend class Iterator;
        friend class TreeIterator;

        PERMIT_SERIALISER_ACCESS  // Allows OOP serialisation
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
