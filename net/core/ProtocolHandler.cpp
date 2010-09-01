// oodles
#include "ProtocolHandler.hpp"

// libc
#include <assert.h> // For assert()

// STL
using std::string;

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
ProtocolHandler::receive_data()
{
    assert(endpoint);

    char *buffer = NULL;
    size_t max = endpoint->inbound.producer().prepare_buffer(buffer);

    endpoint->async_recv(buffer, max);
}

void
ProtocolHandler::transfer_data()
{
    assert(endpoint);

    char *buffer = NULL;
    size_t max = endpoint->outbound.producer().prepare_buffer(buffer), used = 0;

    if (buffer && max > 0)
        used = message2buffer(buffer, max);
        
    max = endpoint->outbound.producer().commit_buffer(used);

    if (max > 0)
        endpoint->async_send(endpoint->outbound.consumer().data(), max);
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
ProtocolHandler::message2buffer(char *&buffer, size_t max)
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
