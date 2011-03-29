// oodles
#include "Session.hpp"
#include "Protocol.hpp"

namespace oodles {
namespace net {
namespace oop {

bool
Session::online() const
{
    Endpoint::Connection e = get_endpoint();
    return e && e->online();
}

void
Session::handle_messages()
{
    Protocol &p = *static_cast<Protocol*>(get_endpoint()->get_protocol());
    
    for (Message *m = p.pop_message() ; m ; m = p.pop_message())
        handle_message(m);
}

void
Session::push_message(Message *m)
{
    Protocol &p = *static_cast<Protocol*>(get_endpoint()->get_protocol());
    p.push_message(m);
}

} // oop
} // net
} // oodles

