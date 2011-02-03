#ifndef OODLES_NET_ONCONNECT_HPP
#define OODLES_NET_ONCONNECT_HPP

namespace oodles {
namespace net {   

class ProtocolHandler; // Forward declaration for OnConnect

struct OnConnect
{
    virtual ~OnConnect() {}
    virtual OnConnect* create() const = 0;
    virtual void operator() (ProtocolHandler &p) = 0;
};
   
} // net   
} // oodles

#endif
