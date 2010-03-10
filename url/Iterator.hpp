#ifndef OODLES_ITERATOR_HPP
#define OODLES_ITERATOR_HPP

// STL
#include <string>
#include <iterator>

namespace oodles {

class URL; // Forward declaration for Iterator

class Iterator :
    public std::iterator<std::bidirectional_iterator_tag,
                         std::string>
{
    private:
        enum {Invalid = 0, Begin, Transient, End};
    public:
        Iterator(const URL *target = NULL, int = Invalid);
        Iterator(const Iterator &rhs);

        Iterator& operator=(const Iterator &rhs);
        Iterator& operator++();
        Iterator& operator--();

        const std::string& operator*()
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
        const std::string *value;
        const URL *url;
        size_t index;
};

} // oodles

#endif
