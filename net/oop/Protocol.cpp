// oodles
#include "Protocol.hpp"

namespace oodles {
namespace net {
namespace oop {

const Message*
Protocol::pop_message()
{
    if (inbound_messages.empty())
        return NULL;

    const Message *m = inbound_messages.front();
    inbound_messages.pop();

    return m;
}

void
Protocol::push_message(const Message *m)
{
    if (m)
        outbound_messages.push(m);
}

void
Protocol::bytes_transferred(size_t n)
{
}

size_t
Protocol::message2buffer(char* &buffer, size_t max)
{
    return 0;
}

size_t
Protocol::buffer2message(const char* &buffer, size_t max)
{
    return 0;
}

} // oop
} // net
} // oodles
