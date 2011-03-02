// oodles
#include "Message.hpp"
#include "Protocol.hpp"

// STL
using std::string;

namespace oodles {
namespace net {
namespace http {

Protocol::Protocol() : incoming(NULL), transferred(0), data_store(0)
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
    if (!m)
        return;

    outbound_messages.push(m);
    
    if (buffered_messages.empty())
        transfer_data();
}

string
Protocol::name() const
{
    return "HTTP";
}

void
Protocol::bytes_transferred(size_t n)
{
    if (!n)
        return;

    Message *outgoing = NULL;
    bool done = buffered_messages.empty();
    size_t message_size = 0, buffered = transferred + n;

    /*
     * Delete all fully-transferred outbound messages
     */
    while (!done) {
        if (!outgoing) {
            outgoing = buffered_messages.front();
            message_size = outgoing->size();
        }

        done = buffered < message_size;

        if (!done) {
            delete outgoing;
            outgoing = NULL;
            buffered_messages.pop();
            buffered -= message_size;
            done = buffered_messages.empty();
        }
    }

    transferred = buffered;
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
        
        if (outgoing->complete(Message::Request)) {
            done = max > 0 && !outbound_messages.empty();
            buffered_messages.push(outgoing);
            outbound_messages.pop();
            outgoing = NULL;
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

    /*
     * Try to buffer as many (last may be partial) messages as we can
     */
    while (!done) {
        if (!incoming)
           incoming = new Message(data_store, 0);

        buffered = incoming->from_buffer(buffer + used, max);
        used += buffered;
        max -= buffered;
        done = max == 0;

        if (incoming->complete(Message::Response)) {
            inbound_messages.push(incoming);
            incoming = NULL;
        }
    }

    return used;
}

} // http
} // net
} // oodles
