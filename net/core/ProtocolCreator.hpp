#ifndef OODLES_NET_PROTOCOLCREATOR_HPP
#define OODLES_NET_PROTOCOLCREATOR_HPP

namespace oodles {
namespace net {

class ProtocolCreator
{
    public:
        /* Member functions/methods */
        virtual ~ProtocolCreator() {}
        virtual ProtocolHandler* create() const = 0;
    private:
        /* Member functions/methods */
        ProtocolCreator(const ProtocolCreator &p);
        ProtocolCreator& operator= (const ProtocolCreator &p);
};

template<typename Handler> struct Protocol : public ProtocolCreator
{
    ProtocolHandler* create() const { return new Handler; }
};

} // net
} // oodles

#endif
