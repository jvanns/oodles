#ifndef OODLES_DISTINCTITEMVECTOR_IPP // Implementation
#define OODLES_DISTINCTITEMVECTOR_IPP

namespace oodles {

template<class T>
uint32_t
DistinctItemVector<T>::item_count() const
{
    return items.count();
}

template<class T>
uint32_t
DistinctItemVector<T>::sequence_count() const
{
    return indicies.size();
}

template<class T>
uint32_t
DistinctItemVector<T>::append_item(const T &item)
{
    Item i(item, locations.size());
    std::pair<typename std::set<Item>::iterator, bool> x = items.insert(i);

    if (x.second) { // New distinct item
        locations.push_back(x.first); // DistinctItemVector the location
    } else { // Already present
        i.index = x.first->index; // Assign the location index
    }

    indicies.push_back(i.index); // Always append the index of the location

    return i.index;
}

template<class T>
bool
DistinctItemVector<T>::retrieve_item(uint32_t index, T &item) const
{
    if (index > indicies.size())
        return false;

    item = locations.at(indicies.at(index))->data;

    return true;
}

} // oodles

#endif
