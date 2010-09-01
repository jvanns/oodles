// oodles
#include "Factory.hpp"
#include "Protocol.hpp"

// STL
using std::string;

namespace oodles {
namespace net {
namespace oop {

Protocol::Protocol(const DialectCreator &c) : ProtocolHandler(c), incoming(NULL)
{
}

Message*
Protocol::pop_message()
{
    if (inbound_messages.empty())
        return NULL;

    Message *m = inbound_messages.front();
    inbound_messages.pop();

    return m;
}

void
Protocol::push_message(Message *m)
{
    if (m)
        outbound_messages.push(m);
}

string
Protocol::name() const
{
    return "OOP";
}

void
Protocol::bytes_transferred(size_t n)
{
}

size_t
Protocol::message2buffer(char* &buffer, size_t max)
{
    if (max == 0 || outbound_messages.empty())
        return 0;
    
    size_t used = 0, buffered = 0;
    Message *m = outbound_messages.front();

    /*
     * Try to buffer as many (last may be partial) messages as we can
     */
    while (m) {
        m->deconstruct();
        buffered = m->to_buffer(buffer + used, max);

        used += buffered;
        max -= buffered;
        
        if (buffered < m->size()) {
            if (buffered == max)
                m = NULL; // Our message size was greater than the buffer size
        }

        if (m) {
            outbound_messages.pop();
            delete m;

            if (max)
                m = outbound_messages.front();
            else
                m = NULL;
        }
    }

    return used;
}

size_t
Protocol::buffer2message(const char *buffer, size_t max)
{
    if (max == 0)
        return 0;
    
    size_t used = 0, buffered = 0;
    static const Factory &factory = Factory::instance();

    while (used < max) {
        if (!incoming) {
            if (max - used >= Message::Header::header_size) {
                const Message::Header h(Message::buffer2header(buffer + used));
                incoming = factory.create(h);
                buffered = h.header_size;
            } else
                max = used; // Terminate the loop safely
        }

        used += buffered;
        buffered += incoming->from_buffer(buffer + used, max - used);

        if (buffered == incoming->size()) {
            inbound_messages.push(incoming);
            incoming->reconstruct();
            incoming = NULL;
        }
    }

    return used;
}

} // oop
} // net
} // oodles
