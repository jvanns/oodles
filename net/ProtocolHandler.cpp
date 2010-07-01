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
ProtocolHandler::transfer_data()
{
    assert(endpoint);

    char *buffer = NULL;
    const size_t max = endpoint->prepare_outbound_buffer(buffer),
                              used = message2buffer(buffer, max);

    assert(used <= max);

    if (used < max)
        endpoint->set_outbound_offset(used); // Allow more writes

    endpoint->async_send();
}

void
ProtocolHandler::start()
{
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
