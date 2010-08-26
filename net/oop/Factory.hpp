#ifndef OODLES_NET_OOP_FACTORY_HPP
#define OODLES_NET_OOP_FACTORY_HPP

// oodles
#include "id_t.hpp"
#include "Message.hpp"
#include "utility/BitSet.hpp"

// STL
#include <vector>

namespace oodles {
namespace net {
namespace oop {

class Factory
{
    public:
        /* Member functions/methods */
        static Factory& instance();

        size_t size() const;
        bool enroll(const Message *m);

        const Message* lookup(size_t subscript) const;
        Message* create(const Message::Header &h) const;
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

