#ifndef OODLES_DISTINCTITEMVECTOR_HPP // Interface
#define OODLES_DISTINCTITEMVECTOR_HPP

// STL
#include <set>
#include <vector>

#include <stdint.h> // For uint32_t

namespace oodles {

template<class T>
class DistinctItemVector
{
    public:
        typedef uint32_t index_t;
    public:
        index_t item_count() const;
        index_t sequence_count() const;
        index_t append_item(const T &item);
        bool retrieve_item(index_t index, T &item) const;
    private:
        struct Item {
            T data;
            index_t index;

            Item(const T &x, index_t i = 0) : data(x), index(i) {}
            bool operator< (const Item &rhs) const { return data < rhs.data; }
        };

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
};

} // oodles

#include "DistinctItemVector.ipp" // Implemention

#endif
