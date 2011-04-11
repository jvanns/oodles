#ifndef OODLES_NET_HTTP_SESSION_HPP
#define OODLES_NET_HTTP_SESSION_HPP

// oodles
#include "utility/Linker.hpp"
#include "net/core/SessionHandler.hpp"

namespace oodles {
namespace net {
namespace http {

class Message; // Forward declaration for Session

class Session : public Linker, public SessionHandler
{
    public:
        void handle_messages();
        void send_request(const std::string &resource, int response_fd);
        void send_response(uint16_t code, int data_fd, size_t data_size);
        
        virtual void handle_message(Message *m) = 0; // Ownership transferred
};
   
} // http
} // net
} // oodles

#endif
