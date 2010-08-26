#ifndef OODLES_NET_OOP_MESSAGES_HPP
#define OODLES_NET_OOP_MESSAGES_HPP

// oodles
#include "Serialiser.hpp"

// STL
#include <string>

// libc
#include <stdint.h> // For uint16_t

namespace oodles {
namespace net {
namespace oop {
namespace msg {

/*
 * This namespace is private. Do not use it. It holds the message
 * implementations as simple structs. However, these are typedef'd
 * later outside of this namespace - use these. These can then be
 * serialised and cast correctly.
 */

struct RegisterCrawler_
{
    /* Member functions/methods */
    static id_t id() { return REGISTER_CRAWLER; }
    void serialize(Reconstructor &archive, unsigned int /* version */);
    void serialize(Deconstructor &archive, unsigned int /* version */) const;
    
    /* Member variables/attributes */
    uint16_t cores;
    std::string name;
};

} // msg

typedef Serialiser<msg::RegisterCrawler_> RegisterCrawler;

} // oop
} // net
} // oodles

#endif
