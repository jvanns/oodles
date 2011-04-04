// oodles
#include "Endpoint.hpp"
#include "CallerContext.hpp"
#include "SessionHandler.hpp"
#include "ProtocolHandler.hpp"

#include "common/Exceptions.hpp"
#include "utility/Dispatcher.hpp"

// Boost.bind
#include <boost/bind.hpp>

// Boost.asio
#include <boost/asio/placeholders.hpp>

// libc
#include <assert.h> // For assert()

// Boost.bind
using boost::bind;

// Boost.system
using boost::system::error_code;

// Boost.asio
using boost::asio::buffer;

namespace oodles {
namespace net {

// Save my fingers!
namespace placeholders = boost::asio::placeholders;

void
Endpoint::Metric::update(size_t bytes)
{
    const time_t now = time(NULL), interval = now - last_transfer;
    const bool update_rate = last_transfer > 0 && interval >= 1;

    transferred_bytes += bytes;

    if (update_rate || (bootstrap && !last_transfer))
        last_transfer = now;

    if (update_rate) { // We only update the rate every second or so
        transfer_rate = (intermediate + bytes) / interval;
        intermediate = 0;
    } else {
        intermediate += bytes;
        bootstrap = false;
    }
}

Endpoint::Endpoint(Dispatcher &d) :
    session(NULL),
    protocol(NULL),
    tcp_socket(d.io_service())
{
    local.hostname = hostname();
}

Endpoint::~Endpoint()
{
    delete session;
    delete protocol;
}

void
Endpoint::stop()
{
    tcp_socket.close();
}

void
Endpoint::start(CallerContext &c)
{
    assert(protocol && session); // Can't do anything without either!
    assert(tcp_socket.is_open()); // Can't do anything with a closed socket!
    /*
     * Disable Nagles algorithm (no_delay) when setting our own buffer sizes
     */
    tcp_socket.set_option(boost::asio::ip::tcp::no_delay(true));
    tcp_socket.set_option(boost::asio::socket_base::keep_alive(true));
    tcp_socket.set_option(boost::asio::socket_base::send_buffer_size(NBS));
    tcp_socket.set_option(boost::asio::socket_base::receive_buffer_size(NBS));

    local.port = tcp_socket.local_endpoint().port();
    local.ip = tcp_socket.local_endpoint().address().to_string();
    
    remote.port = tcp_socket.remote_endpoint().port();
    remote.ip = tcp_socket.remote_endpoint().address().to_string();

    session->start(c); // Call first to prepare session/context before transfers
    protocol->start();
}

void
Endpoint::set_remote_fqdn(const std::string &fqdn)
{
    remote.hostname = fqdn;
}

void
Endpoint::set_session(SessionHandler *s)
{
    assert(!session); // Must be set only once

    s->set_endpoint(shared_from_this());

    session = s;
}

void
Endpoint::set_protocol(ProtocolHandler *p)
{
    assert(!protocol); // Must be set only once

    p->set_endpoint(shared_from_this());
    
    protocol = p; // Ownership is transferred
}

void
Endpoint::async_recv(char *ptr, size_t max)
{
    assert(protocol);

    if (!(ptr && max > 0))
        return;

    tcp_socket.async_read_some(buffer(ptr, max),
                               bind(&Endpoint::raw_recv_callback,
                               shared_from_this(),
                               placeholders::error,
                               placeholders::bytes_transferred));
}

void
Endpoint::async_send(const char *ptr, size_t max)
{
    assert(protocol);

    if (!(ptr && max > 0))
        return;

    tcp_socket.async_write_some(buffer(ptr, max),
                                bind(&Endpoint::raw_send_callback,
                                shared_from_this(),
                                placeholders::error,
                                placeholders::bytes_transferred));
}

void
Endpoint::raw_recv_callback(const error_code& e, size_t b)
{
    if (!e && b > 0) {
        size_t n = inbound.producer().commit_buffer(b),
               u = protocol->buffer2message(inbound.consumer().data(), n);
        
        inbound.consumer().consume_buffer(u);
        protocol->receive_data();
        recv_rate.update(b);
    } else {
        if (e == boost::asio::error::operation_aborted)
            return;
        
        stop();
    
        if (e != boost::asio::error::eof)
            throw ReadError("Endpoint::raw_recv_callback",
                            e.value(),
                            e.message().c_str());
    }
}

void
Endpoint::raw_send_callback(const error_code& e, size_t b)
{
    if (!e && b > 0) {
        size_t n = outbound.consumer().consume_buffer(b);
        
        protocol->bytes_transferred(b);
        protocol->transfer_data(n); 
        send_rate.update(b);
    } else {
        if (e == boost::asio::error::operation_aborted)
            return;
        
        stop();
        
        if (e != boost::asio::error::eof)
            throw WriteError("Endpoint::raw_send_callback",
                             e.value(),
                             e.message().c_str());
    }
}

} // net
} // oodles

