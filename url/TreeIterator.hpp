#ifndef OODLES_URL_TREEITERATOR_HPP
#define OODLES_URL_TREEITERATOR_HPP

// STL
#include <string>
#include <iterator>

namespace oodles {
namespace url {

class URL; // Forward declaration for Iterator

class TreeIterator :
    public std::iterator<std::bidirectional_iterator_tag,
                         std::string>
{
    private:
        enum {Invalid = 0, Begin, Transient, End};
    public:
        TreeIterator(const URL *target = NULL, int = Invalid);
        TreeIterator(const TreeIterator &rhs);

        TreeIterator& operator=(const TreeIterator &rhs);
        TreeIterator& operator++();
        TreeIterator& operator--();

        const std::string& operator*()
        {
            return *value;
        }

        bool operator==(const TreeIterator &rhs) const
        {
            return value == rhs.value;
        }

        bool operator!=(const TreeIterator &rhs) const
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

} // url
} // oodles

#endif
