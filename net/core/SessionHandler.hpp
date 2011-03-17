#ifndef OODLES_NET_SESSIONHANDLER_HPP
#define OODLES_NET_SESSIONHANDLER_HPP

// oodles
#include "Endpoint.hpp"

namespace oodles {
namespace net {

class SessionHandler
{
    public:
        virtual ~SessionHandler() {}
        virtual void begin() = 0;
        virtual void end() = 0;

        void set_endpoint(Endpoint::Connection e);
        Endpoint::Connection get_endpoint() const { return endpoint; }
    private:
        Endpoint::Connection endpoint;
};
   
} // net
} // oodles

#endif

