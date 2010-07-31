#ifndef OODLES_NET_PROTOCOLCREATOR_HPP
#define OODLES_NET_PROTOCOLCREATOR_HPP

// Boost.shared_ptr
#include <boost/shared_ptr.hpp>

namespace oodles {
namespace net {

struct ProtocolHandler; // Forward declaration for ProtocolCreator

class ProtocolCreator
{
    public:
        /* Member functions/methods */
        ProtocolCreator() {}
        virtual ~ProtocolCreator() {}
        virtual ProtocolHandler* create() const = 0;
    private:
        ProtocolCreator(const ProtocolCreator &p);
        ProtocolCreator& operator= (const ProtocolCreator &p);
};

template<typename DerivedHandler> class Protocol : public ProtocolCreator
{
    private:
        /* Dependent typedefs */
        typedef boost::shared_ptr<ProtocolHandler> Base;
    public:
        /* Dependent typedefs */
        typedef boost::shared_ptr<DerivedHandler> Handler;

        /* Member functions/methods */
        ProtocolHandler* create() const
        {
           return new DerivedHandler;
        }

        Handler handler(const Base &b) const
        {
            return boost::static_pointer_cast
                   <DerivedHandler, ProtocolHandler>(b);
        }
};

} // net
} // oodles

#endif
