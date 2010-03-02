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
        enum State {Invalid = 0, Begin, Transient, End };
    public:
        Iterator(const URL *url = NULL, State = Invalid);
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
        friend class URL; // URL wants access only to State
    private:
        const std::string *value;
        const URL *url;
        size_t index;
        State state;
};

} // oodles

#endif
