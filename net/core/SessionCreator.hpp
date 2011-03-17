#ifndef OODLES_NET_SESSIONCREATOR_HPP
#define OODLES_NET_SESSIONCREATOR_HPP

namespace oodles {
namespace net {

class SessionHandler; // Forward declaration for SessionCreator

} // net
} // oodles

namespace private_session {

class CreatorBase
{
    public:
        /* Member functions/methods */
        CreatorBase() {}
        virtual ~CreatorBase() {}
        virtual oodles::net::SessionHandler* create() const = 0;
    private:
        /* Member functions/methods */
        CreatorBase(const CreatorBase &p);
        CreatorBase& operator= (const CreatorBase &p);
};

} // private_session

namespace oodles {
namespace net {

template<typename Handler>
struct SessionCreator : public private_session::CreatorBase
{
    SessionHandler* create() const { return new Handler; }
};

} // net
} // oodles

#endif
