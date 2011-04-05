#ifndef OODLES_NET_SESSIONHANDLER_HPP
#define OODLES_NET_SESSIONHANDLER_HPP

// oodles
#include "Endpoint.hpp"
#include "CallerContext.hpp"

namespace oodles {
namespace net {

class SessionHandler
{
    public:
        virtual ~SessionHandler() {}
        virtual void handle_messages() {}

        void set_endpoint(Endpoint::Connection e);
        void print_metrics(std::ostream *s) const;
        
        inline void start(CallerContext &c) { c.start(*this); }
        Endpoint::Connection get_endpoint() const { return endpoint; }
    private:
        Endpoint::Connection endpoint;
};
   
} // net
} // oodles

#endif

