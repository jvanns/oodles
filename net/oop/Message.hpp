#ifndef OODLES_NET_OOP_MESSAGE_HPP
#define OODLES_NET_OOP_MESSAGE_HPP

// oodles
#include "id_t.hpp"

namespace oodles {
namespace net {
namespace oop {

class Message
{
    public:
        /* Dependent typedefs */
        typedef uint32_t sz_t;

        /* Member functions/methods */
        Message();
        virtual ~Message();
        
        sz_t body_size() const;
        sz_t size() const { return header_size() + body_size(); }
        static uint16_t header_size() { return sizeof(id_t) + sizeof(sz_t); }

        virtual id_t id() const = 0;
};

} // oop
} // net
} // oodles

#endif

