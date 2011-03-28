// oodles
#include "Session.hpp"
#include "Protocol.hpp"

namespace oodles {
namespace net {
namespace oop {

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

