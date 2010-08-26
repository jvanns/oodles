#ifndef OODLES_NET_OOP_ID_T_HPP
#define OODLES_NET_OOP_ID_T_HPP

// libc
#include <stdint.h> // For uint32_t

namespace oodles {
namespace net {
namespace oop {

typedef uint16_t id_t;

enum {
    INVALID_ID = 0,

    /*
     * Message IDs for OOP dialog from a Crawler to a Scheduler (one way C->S)
     */
    REGISTER_CRAWLER = 10,
    
    MAX_MESSAGE_ID // Always maintain this as the last item
};
   
} // oop
} // net
} // oodles

#endif
