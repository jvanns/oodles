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
    /*
     * FIXME: In message2buffer transfer the message written to the buffer
     * to a new queue called 'buffered' or something. Then from this method
     * check the message at the front of the buffered queue and it's size
     * against the number of bytes (n) that were sent. If n >= size() then
     * delete the message and move on to the next.
     */
}

size_t
Protocol::message2buffer(char *buffer, size_t max)
{
    if (max == 0 || outbound_messages.empty())
        return 0;
    
    bool done = false;
    Message *outgoing = NULL;
    size_t used = 0, buffered = 0;
    
    /*
     * Try to buffer as many (last may be partial) messages as we can
     */
    while (!done) {
        if (!outgoing)
            outgoing = outbound_messages.front();

        buffered = outgoing->to_buffer(buffer + used, max);
        used += buffered;
        max -= buffered;
        done = max == 0;
        
        if (!outgoing->pending()) {
            delete outgoing;
            outgoing = NULL;
            outbound_messages.pop();
            done = !(!outbound_messages.empty() && max > 0);
        }
    }

    return used;
}

size_t
Protocol::buffer2message(const char *buffer, size_t max)
{
    if (max == 0)
        return 0;
    
    bool done = false;
    size_t used = 0, buffered = 0;
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
        done = max == 0;

        if (!incoming->pending()) {
            done = max < Message::Header::header_size;
            inbound_messages.push(incoming);
            incoming = NULL;
        }
    }
    
    if (!inbound_messages.empty() && dialect != this)
        dialect->translate();

    return used;
}

} // oop
} // net
} // oodles
