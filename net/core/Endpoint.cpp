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

Endpoint::Endpoint(io_service &s) : tcp_socket(s)
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
    tcp_socket.set_option(boost::asio::socket_base::send_buffer_size(OBS));
    tcp_socket.set_option(boost::asio::socket_base::receive_buffer_size(IBS));
    
    protocol->start(); // May prepare messages for sending

    async_send();
    async_recv();
}

void
Endpoint::set_protocol(Protocol p)
{
    assert(!protocol); // Must be set only once

    p->set_endpoint(shared_from_this());
    protocol = p;
}

void
Endpoint::async_recv()
{
    assert(protocol);

    if (inbound.locked || inbound.full())
        return;

    tcp_socket.async_read_some(buffer(inbound.data(), inbound.size()),
                                      bind(&Endpoint::raw_recv_callback,
                                      shared_from_this(),
                                      placeholders::error,
                                      placeholders::bytes_transferred));

    inbound.locked = true;
}

void
Endpoint::async_send()
{
    assert(protocol);

    if (outbound.locked || outbound.empty())
        return;

    tcp_socket.async_write_some(buffer(outbound.data(), outbound.offset),
                                       bind(&Endpoint::raw_send_callback,
                                       shared_from_this(),
                                       placeholders::error,
                                       placeholders::bytes_transferred));

    outbound.locked = true;
}

void
Endpoint::raw_recv_callback(const error_code& e, size_t b)
{
    if (!e) {
        size_t n = inbound.offset + b; // Raw, readable data
        size_t r = protocol->buffer2message(inbound.data(), n);

        assert(r <= n);

        inbound.consume(n - r); // Data will be invalidated
        inbound.offset = r; // Data remaining after any message processing

        inbound.locked = false;

        async_recv(); // Ready the socket for more data
    } else {
        if (e != boost::asio::error::operation_aborted)
            stop();
    }
}

void
Endpoint::raw_send_callback(const error_code& e, size_t b)
{
    if (!e) {
        outbound.consume(b); // Always remove sent data from the buffer
        outbound.offset -= b; // We may still have some data left to send
        
        protocol->bytes_transferred(b); // Inform the handler how much was sent

        outbound.locked = false;

        async_send(); // Continue sending if buffer still contains data
    } else {
        if (e != boost::asio::error::operation_aborted)
            stop();
    }
}

size_t
Endpoint::prepare_outbound_buffer(byte_t *&buffer)
{
    buffer = outbound.buffer.begin() + outbound.offset;
    outbound.offset = outbound.size() - outbound.offset;

    return outbound.offset;
}

} // net
} // oodles

