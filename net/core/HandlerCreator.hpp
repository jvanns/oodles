#ifndef OODLES_NET_HANDLERCREATOR_HPP
#define OODLES_NET_HANDLERCREATOR_HPP

// oodles
#include "CallerContext.hpp"
#include "SessionCreator.hpp"
#include "ProtocolCreator.hpp"

namespace oodles {
namespace net {

class HandlerCreator
{
    public:
        HandlerCreator(CallerContext &c) : context(c) {}
        virtual ~HandlerCreator() {}

        virtual SessionHandler* create_session() const = 0;
        virtual ProtocolHandler* create_protocol() const = 0;

        inline CallerContext& caller_context() const { return context; }
    private:
        /* Member variables/attributes */
        CallerContext &context;
        
        /* Member functions/methods */
        HandlerCreator(const HandlerCreator &c);
        HandlerCreator& operator= (const HandlerCreator &c);
};

template<typename PH, typename SH> struct Creator : public HandlerCreator
{

    Creator(CallerContext &c) : HandlerCreator(c) {}
    
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



