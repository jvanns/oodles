// oodles
#include "Endpoint.hpp"
#include "ProtocolHandler.hpp"
#include "common/Exceptions.hpp"

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

Endpoint::Endpoint(io_service &s) : protocol(NULL), tcp_socket(s)
{
}

Endpoint::~Endpoint()
{
    delete protocol;
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
    /*
     * Disable Nagles algorithm (no_delay) when setting our own buffer sizes
     */
    tcp_socket.set_option(boost::asio::ip::tcp::no_delay(true));
    tcp_socket.set_option(boost::asio::socket_base::keep_alive(true));
    tcp_socket.set_option(boost::asio::socket_base::send_buffer_size(NBS));
    tcp_socket.set_option(boost::asio::socket_base::receive_buffer_size(NBS));

    protocol->start();
}

void
Endpoint::set_protocol(ProtocolHandler *p)
{
    assert(!protocol); // Must be set only once

    extend_link_to(p);
    p->extend_link_to(p->get_dialect());
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
        const time_t now = time(NULL);
        size_t n = inbound.producer().commit_buffer(b),
               u = protocol->buffer2message(inbound.consumer().data(), n);
        
        inbound.consumer().consume_buffer(u);
        protocol->receive_data();
        
        /*
         * Update recv metrics
         */
        if (!recv_rate)
            recv_rate.transfer_rate = recv_rate.transferred_bytes = b;
        else {
            recv_rate.transfer_rate = (recv_rate.transferred_bytes + b) /
                                      ((recv_rate.last_transfer - now) + 1);
            recv_rate.transferred_bytes += b;
            recv_rate.transfer_rate /= 60;
        }
        
        recv_rate.last_transfer = now;
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
        const time_t now = time(NULL);
        
        protocol->bytes_transferred(b);
        protocol->transfer_data(n);
        
        /*
         * Update send metrics
         */
        if (!send_rate)
            send_rate.transfer_rate = send_rate.transferred_bytes = b;
        else {
            send_rate.transfer_rate = (send_rate.transferred_bytes + b) /
                                      ((send_rate.last_transfer - now) + 1);
            send_rate.transferred_bytes += b;
            send_rate.transfer_rate /= 60;
        }
        
        send_rate.last_transfer = now;
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

