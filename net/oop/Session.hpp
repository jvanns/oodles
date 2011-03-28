#ifndef OODLES_NET_OOP_SESSION_HPP
#define OODLES_NET_OOP_SESSION_HPP

// oodles
#include "utility/Linker.hpp"
#include "net/core/SessionHandler.hpp"

namespace oodles {
namespace net {
namespace oop {

class Message; // Forward declaration for Session

class Session : public Linker, public SessionHandler
{
    public:
        void handle_messages();
        
        void push_message(Message *m); // Ownership transferred
        virtual void handle_message(Message *m) = 0; // Ownership transferred
};
   
} // oop
} // net
} // oodles

#endif
