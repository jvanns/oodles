#ifndef OODLES_DISTINCTITEM_IPP // Implementation
#define OODLES_DISTINCTITEM_IPP

// libc
#include <assert.h> // For assert()

namespace oodles {

template<class Type, class Compare>
DistinctItem<Type, Compare>::DistinctItem(bool unique) : unique(unique)
{
}

template<class Type, class Compare>
std::pair<size_t, bool>
DistinctItem<Type, Compare>::insert(const Type &item)
{
    Item i(item, locations.size());
    std::pair<typename std::set<Item>::iterator, bool> x(items.insert(i));

    if (x.second) { // New distinct item
        locations.push_back(x.first); // Store the location within the set<>
    } else { // Already present
        i.index = x.first->index; // Assign the location index
    }

    /*
     * If the item was newly inserted into the set or we are
     * storing multiple pointers to it then we store the index
     * to it's iterator for random access via operator[].
     */
    if (!unique || x.second)
        indicies.push_back(i.index); // Always append the index of the location

    return std::pair<size_t, bool>(i.index, x.second);
}

template<class Type, class Compare>
Type&
DistinctItem<Type, Compare>::item(size_t i)
{
    assert(i >= 0 && i < size());
    return const_cast<Type&>(locations.at(indicies.at(i))->data);
}

template<class Type, class Compare>
const Type&
DistinctItem<Type, Compare>::item(size_t i) const
{
    assert(i >= 0 && i < size());
    return locations.at(indicies.at(i))->data;
}

} // oodles

#endif
