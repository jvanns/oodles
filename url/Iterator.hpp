#ifndef OODLES_URL_ITERATOR_HPP
#define OODLES_URL_ITERATOR_HPP

// oodles
#include "type.hpp"

// STL
#include <iterator>

namespace oodles {
namespace url {

class URL; // Forward declaration for Iterator

class Iterator :
    public std::iterator<std::bidirectional_iterator_tag, value_type>
{
    private:
        enum {Invalid = 0, Begin, Transient, End};
    public:
        Iterator(const URL *target = NULL, int = Invalid);
        Iterator(const Iterator &rhs);

        Iterator& operator=(const Iterator &rhs);
        Iterator& operator++();
        Iterator& operator--();

        const value_type& operator*()
        {
            return *value;
        }

        bool operator==(const Iterator &rhs) const
        {
            return value == rhs.value;
        }

        bool operator!=(const Iterator &rhs) const
        {
            return value != rhs.value;
        }
    private:
        bool move_pointer_forward();
        bool move_pointer_backward();
    private:
        friend class URL; // URL wants access only to enums above
    private:
        int internal_state, external_state;
        const value_type *value;
        const URL *url;
        size_t index;
};

} // url
} // oodles

#endif
