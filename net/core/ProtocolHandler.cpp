// oodles
#include "ProtocolHandler.hpp"

// libc
#include <assert.h> // For assert()

// STL
using std::string;
using std::ostream;

namespace oodles {
namespace net {

// Public methods
ProtocolHandler::~ProtocolHandler()
{
}

void
ProtocolHandler::set_endpoint(Endpoint::Connection e)
{
    assert(!endpoint);
    endpoint = e;
}

void
ProtocolHandler::start()
{
    transfer_data();
    receive_data();
}

void
ProtocolHandler::receive_data()
{
    assert(endpoint);
    
    if (!endpoint->online())
        return; // Not to worry, receive_data() will be called again by start()

    char *buffer = NULL;
    size_t max = endpoint->inbound.producer().prepare_buffer(buffer);

    endpoint->async_recv(buffer, max);
}

void
ProtocolHandler::transfer_data(size_t pending)
{
    assert(endpoint);

    if (!endpoint->online())
        return; // Not to worry, transfer_data() will be called again by start()

    if (!pending) {
        char *buffer = NULL;
        size_t max = endpoint->outbound.producer().prepare_buffer(buffer);

        if (buffer && max) {
            pending = message2buffer(buffer, max);
            max = endpoint->outbound.producer().commit_buffer(pending);
        }

        pending = max;
    }

    endpoint->async_send(endpoint->outbound.consumer().data(), pending);
}

void
ProtocolHandler::print_metrics(ostream *s) const
{
    assert(endpoint);

    if (!s)
        return;

    *s << "TCP:\n"
       << "Received (KiB):       "
       << endpoint->recv_rate.transferred_bytes / 1024 << '\n'
       << "Last transfer:        "
       << endpoint->recv_rate.last_transfer << '\n'
       << "Receive rate (KiB/s): " << endpoint->recv_rate.transfer_rate / 1024.f
       << '\n';
    
    *s << "Sent (KiB):           "
       << endpoint->send_rate.transferred_bytes / 1024 << '\n'
       << "Last transfer:        "
       << endpoint->send_rate.last_transfer << '\n'
       << "Send rate (KiB/s):    " << endpoint->send_rate.transfer_rate / 1024.f
       << '\n';
}

string
ProtocolHandler::name() const
{
    return "";
}

void
ProtocolHandler::bytes_transferred(size_t n)
{
}

size_t
ProtocolHandler::message2buffer(char *buffer, size_t max)
{
    return 0;
}

size_t
ProtocolHandler::buffer2message(const char *buffer, size_t max)
{
    return 0;
}

} // net
} // oodles
