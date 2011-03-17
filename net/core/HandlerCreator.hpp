#ifndef OODLES_NET_HANDLERCREATOR_HPP
#define OODLES_NET_HANDLERCREATOR_HPP

// oodles
#include "SessionCreator.hpp"
#include "ProtocolCreator.hpp"

namespace oodles {
namespace net {

class HandlerCreator
{
    public:
        HandlerCreator() {}
        virtual ~HandlerCreator() {}

        virtual SessionHandler* create_session() const = 0;
        virtual ProtocolHandler* create_protocol() const = 0;
    private:
        HandlerCreator(const HandlerCreator &c);
        HandlerCreator& operator= (const HandlerCreator &c);
};

template<typename SH, PH> struct Creator : public HandlerCreator
{
    typedef SH Session;
    typedef PH Protocol;
    
    SessionHandler* create_session() const
    {
        static const SessionCreator<SH> x;
        return x.create();
    }
    
    ProtocolHandler* create_protocol() const
    {
        static const ProtocolCreator<PH> x;
        return x.create();
    }
};
   
} // net
} // oodles

#endif



