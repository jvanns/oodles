// oodles
#include "SessionHandler.hpp"
#include "ProtocolHandler.hpp"
#include "utility/Dispatcher.hpp"

// Boost.bind
#include <boost/bind.hpp>

// libc
#include <assert.h> // For assert()

// STL
using std::string;
using std::ostream;

// Boost
using boost::bind;

namespace oodles {
namespace net {

// Public methods
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
ProtocolHandler::handle_messages(Dispatcher &d) const
{
    SessionHandler *s = endpoint->get_session();
    d.io_service().post(bind(&SessionHandler::handle_messages, s));
}

void
ProtocolHandler::print_metrics(ostream *s) const
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
