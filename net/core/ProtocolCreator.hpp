#ifndef OODLES_NET_PROTOCOLCREATOR_HPP
#define OODLES_NET_PROTOCOLCREATOR_HPP

namespace oodles {
namespace net {

class ProtocolHandler; // Forward declaration for ProtocolCreator

} // net
} // oodles

namespace private_protocol {

class CreatorBase
{
    public:
        /* Member functions/methods */
        CreatorBase() {}
        virtual ~CreatorBase() {}
        virtual oodles::net::ProtocolHandler* create() const = 0;
    private:
        /* Member functions/methods */
        CreatorBase(const CreatorBase &p);
        CreatorBase& operator= (const CreatorBase &p);
};

} // private_protocol

namespace oodles {
namespace net {

template<typename Handler>
struct ProtocolCreator : public private_protocol::CreatorBase
{
    ProtocolHandler* create() const { return new Handler; }
};

} // net
} // oodles

#endif
