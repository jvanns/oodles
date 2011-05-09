#ifndef OODLES_NET_HTTP_PROTOCOL_HPP
#define OODLES_NET_HTTP_PROTOCOL_HPP

// oodles
#include "net/core/ProtocolHandler.hpp"

// STL
#include <queue>

namespace oodles {
namespace net {
namespace http {

class Message; // Forward declaration for Protocol
 
class Protocol : public ProtocolHandler
{
    public:
        /* Member functions/methods */
        Protocol();
        
        Message* pop_message();
        void push_message(Message *m, int response_fd = -1);
        
        /*
         * These methods override the pure virtual interface
         * provided by the base class, ProtocolHandler.
         */
        std::string name() const;
        void bytes_transferred(size_t n);
        size_t message2buffer(char *buffer, size_t max);
        size_t buffer2message(const char *buffer, size_t max);
    private:
        /* Member functions/methods */
        int response_fd();
        
        /* Member variables/attributes */
        Message *incoming;
        size_t transferred;
        std::queue<int> response_fds;
        std::queue<Message*> inbound_messages,
                             outbound_messages,
                             buffered_messages;
};
 
} // http
} // net
} // oodles

#endif

