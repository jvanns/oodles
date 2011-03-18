#ifndef OODLES_NET_CALLERCONTEXT_HPP
#define OODLES_NET_CALLERCONTEXT_HPP

namespace oodles {
namespace net {

class SessionHandler; // Forward declaration for CallerContext

struct CallerContext
{
    virtual ~CallerContext() {}
    virtual void connect_event(SessionHandler *s) = 0;
};

} // net
} // oodles

#endif
