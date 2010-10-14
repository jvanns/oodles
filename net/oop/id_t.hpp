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
     * Message IDs for OOP dialect between Crawler and Scheduler
     */
    REGISTER_CRAWLER = 10, // Crawler registers itself for work with Scheduler
    BEGIN_CRAWL, // Scheduler assigns URLs to crawler mandating a crawl
    END_CRAWL, // Crawler informs the scheduler of completed crawls
    
    MAX_MESSAGE_ID // Always maintain this as the last item
};
   
} // oop
} // net
} // oodles

#endif
