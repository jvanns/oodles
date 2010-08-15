// oodles
#include "Endpoint.hpp"
#include "ProtocolHandler.hpp"

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
using boost::asio::io_service;

namespace oodles {
namespace net {

// Save my fingers!
namespace placeholders = boost::asio::placeholders;

Endpoint::Endpoint(io_service &s) :
    tcp_socket(s),
    read_strand(s),
    write_strand(s)
{
}

Endpoint::~Endpoint()
{
}

void
Endpoint::stop()
{
    tcp_socket.close();
}

void
Endpoint::start()
{
    assert(protocol); // Can't do anything without a protocol
    assert(tcp_socket.is_open()); // Can't do anything with a closed socket!

    tcp_socket.set_option(boost::asio::socket_base::keep_alive(true));
    
    protocol->transfer_data();
    protocol->receive_data();
}

void
Endpoint::set_protocol(Protocol p)
{
    assert(!protocol); // Must be set only once

    p->set_endpoint(shared_from_this());
    protocol = p;
}

void
Endpoint::async_recv(char *ptr, size_t max)
{
    assert(protocol);

    if (!(ptr && max > 0))
        return;

    tcp_socket.async_read_some(buffer(ptr, max),
                                      read_strand.wrap(
                                      bind(&Endpoint::raw_recv_callback,
                                      shared_from_this(),
                                      placeholders::error,
                                      placeholders::bytes_transferred)));
}

void
Endpoint::async_send(const char *ptr, size_t max)
{
    assert(protocol);

    if (!(ptr && max > 0))
        return;

    tcp_socket.async_write_some(buffer(ptr, max),
                                       write_strand.wrap(
                                       bind(&Endpoint::raw_send_callback,
                                       shared_from_this(),
                                       placeholders::error,
                                       placeholders::bytes_transferred)));
}

void
Endpoint::raw_recv_callback(const error_code& e, size_t b)
{
    if (!e && b > 0) {
        size_t n = inbound.producer().commit_buffer(b),
               u = protocol->buffer2message(inbound.consumer().
                                            yield_buffer(), n);

        inbound.consumer().consume_buffer(u);
        protocol->receive_data();
    } else {
        if (e != boost::asio::error::operation_aborted)
            stop();
    }
}

void
Endpoint::raw_send_callback(const error_code& e, size_t b)
{
    if (!e && b > 0) {
        size_t r = outbound.consumer().consume_buffer(b);

        if (r)
            async_send(outbound.consumer().yield_buffer(), r);

        protocol->bytes_transferred(b); // Inform the handler how much was sent
    } else {
        if (e != boost::asio::error::operation_aborted)
            stop();
    }
}

} // net
} // oodles

