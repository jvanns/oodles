#ifndef OODLES_NET_OOP_FACTORY_HPP
#define OODLES_NET_OOP_FACTORY_HPP

// oodles
#include "utility/BitSet.hpp"

// STL
#include <vector>

namespace oodles {
namespace net {
namespace oop {

class Message; // Forward declaration for Factory

class Factory
{
    public:
        /* Member functions/methods */
        static Factory& instance();

        size_t size() const;
        bool enroll(const Message *m);

        Message* operator[] (size_t subscript);
        const Message* operator[] (size_t subscript) const;
    private:
        /* Member functions/methods */
        Factory();
        ~Factory();
        
        /* Member variables/attributes */
        BitSet index; // Our index of messages in registry
        std::vector<const Message*> registry;
};

} // oop
} // net
} // oodles

#endif

