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
    m->reconstruct();

    return m;
}

void
Protocol::push_message(Message *m)
{
    if (m) {
        outbound_messages.push(m);
        m->deconstruct();
        transfer_data();
    }
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
Protocol::message2buffer(char *buffer, size_t max)
{
    if (max == 0 || outbound_messages.empty())
        return 0;
    
    size_t used = 0, buffered = 0;
    Message *m = outbound_messages.front();

    /*
     * Try to buffer as many (last may be partial) messages as we can
     */
    while (m) {
        buffered = m->to_buffer(buffer + used, max);
        
        if (m->pending() && buffered == max)
            m = NULL; // Our message size was greater than the buffer size

        used += buffered;
        max -= buffered;

        if (m) {
            outbound_messages.pop();
            delete m;

            if (max && !outbound_messages.empty())
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
    bool done = max < Message::Header::header_size;
    static const Factory &factory = Factory::instance();

    while (!done) {
        if (!incoming) {
            const Message::Header h(Message::buffer2header(buffer + used));
            incoming = factory.create(h);
            used += h.header_size;
            max -= h.header_size;
        }
        
        buffered = incoming->from_buffer(buffer + used, max);
        used += buffered;
        max -= buffered;

        if (!incoming->pending()) {
            inbound_messages.push(incoming);
            incoming = NULL;
        }
        
        done = !max || max < Message::Header::header_size;
    }
    
    if (!inbound_messages.empty() && dialect != this)
        dialect->translate();

    return used;
}

} // oop
} // net
} // oodles
