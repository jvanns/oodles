#ifndef OODLES_URL_URL_HPP
#define OODLES_URL_URL_HPP

// oodles
#include "Parser.hpp"
#include "Iterator.hpp"
#include "TreeIterator.hpp"
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
        URL(const std::string &url);

        bool operator== (const URL &rhs) const;
        bool operator!= (const URL &rhs) const;

        hash_t page_id();
        hash_t path_id();
        hash_t domain_id();
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
        template<class Type>
        class IDGenerator
        {
            public:
                /* Member functions/methods */
                IDGenerator(const Type &value);
                ~IDGenerator();

                hash_t id(hash_t seed) { return compute_hash(seed); }
            private:
                /* Member functions/methods */
                hash_t compute_hash(hash_t seed);

                /* Member variables/attributes */
                hash_t hash;
                const Type &content;
        };

        /* Member functions/methods */
        void tokenise(const std::string &url) throw(ParseError);

        /* Member variables/attributes */
        Attributes attributes;
        IDGenerator<value_type> page;
        IDGenerator<std::vector<value_type> > path, domain;

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

#include "URL.ipp" // Implementation (IDGenerator only)

#endif
