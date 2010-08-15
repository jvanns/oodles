#ifndef OODLES_INDEXEDSET_HPP // Interface
#define OODLES_INDEXEDSET_HPP

// STL
#include <set>
#include <vector>

// libc
#include <stdint.h> // For uint32_t

namespace oodles {

template<class Type, class Compare = std::less<Type> >
class IndexedSet
{
    public:
        /* Member functions/methods */
        IndexedSet(bool unique = false);
        std::pair<size_t, bool> insert(const Type &item);

        size_t size() const { return indicies.size(); }
        size_t duplicates() const { return indicies.size() - items.size(); }

        Type& operator[] (size_t i) { return item(i); }
        const Type& operator[] (size_t i) const { return item(i); }
    private:
        /* Internal Data Structures */
        struct Item {
            Type data;
            Compare cmp;
            size_t index;

            Item(const Type &x, size_t i = 0) : data(x), index(i) {}
            bool operator< (const Item &rh) const { return cmp(data, rh.data); }
        };

        /* Member variables/attributes */

        /*
         * If true all containers will be distinct. That is, not only
         * will the items be unique but pointers to them (the indicies
         * and iterator locations) will also not be duplicated.
         */
        bool unique;

        /*
         * Our distinct set of items
         */
        std::set<Item> items;

        /*
         * A vector of the indicies of each
         * item location. This container
         * will grow to the size of the no.
         * of input items (inc. duplicates).
         */
        std::vector<uint32_t> indicies;

        /*
         * The locations of each item in the
         * set of distinct items. No location
         * is duplicated and the subscript of
         * each location forms the index above.
         */
        std::vector<typename std::set<Item>::iterator> locations;

        /* Member functions/methods */
        Type& item(size_t i);
        const Type& item(size_t i) const;
};

} // oodles

#include "IndexedSet.ipp" // Implemention

#endif
