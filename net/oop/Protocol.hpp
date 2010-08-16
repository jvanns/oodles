#ifndef OODLES_NET_OOP_PROTOCOL_HPP
#define OODLES_NET_OOP_PROTOCOL_HPP

// oodles
#include "net/core/ProtocolHandler.hpp"

// STL
#include <queue>

namespace oodles {
namespace net {
namespace oop {

class Message; // Forward declaration for Protocol
 
class Protocol : public ProtocolHandler
{
    public:
        /* Member functions/methods */
        const Message* pop_message();
        void push_message(const Message *m);
        
        void bytes_transferred(size_t n);
        size_t message2buffer(char* &buffer, size_t max);
        size_t buffer2message(const char* &buffer, size_t max);
    private:
        /* Member variables/attributes */
        std::queue<const Message*> inbound_messages, outbound_messages;
};
   
} // oop
} // net
} // oodles

#endif

