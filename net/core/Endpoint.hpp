#ifndef OODLES_NET_ENDPOINT_HPP
#define OODLES_NET_ENDPOINT_HPP

// oodles
#include "Buffer.hpp"

// Boost.shared
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// Boost.asio
#include <boost/asio/ip/tcp.hpp>

// libc
#include <stdint.h> // For uint16_t

namespace {
    
static const uint32_t MIN_BUFFER_SIZE = 4096; // 4KiB
static const uint32_t MAX_BUFFER_SIZE = 1024 * 1024; // 1MiB

} // anonymous

namespace oodles {
namespace net {

static const uint16_t OBS =  /* Outbound buffer size */
#ifdef OUTBOUND_BUFFER_SIZE
       OUTBOUND_BUFFER_SIZE <= MAX_BUFFER_SIZE
       ? OUTBOUND_BUFFER_SIZE
       : MAX_BUFFER_SIZE;
#else
       MIN_BUFFER_SIZE;
#endif

static const uint16_t IBS = /* Inbound buffer size */
#ifdef INBOUND_BUFFER_SIZE
       INBOUND_BUFFER_SIZE <= MAX_BUFFER_SIZE
       ? INBOUND_BUFFER_SIZE
       : MAX_BUFFER_SIZE;
#else
       MIN_BUFFER_SIZE;
#endif

class ProtocolHandler; // Forward declaration for Endpoint

class Endpoint : public boost::enable_shared_from_this<Endpoint>
{
    public:
        /* Dependent typedefs */
        typedef boost::shared_ptr<Endpoint> Connection;

        /* Member functions/methods */
        ~Endpoint();

        static
        Endpoint*
        create(boost::asio::io_service &s)
        {
            return new Endpoint(s);
        }

        bool online() const { return tcp_socket.is_open(); }
        boost::asio::ip::tcp::socket& socket() { return tcp_socket; }

        void stop(); // Close socket cancelling pending handlers
        void start(); // Must be called to register reads/writes
        void set_protocol(ProtocolHandler *p); // Pair protocol with endpoint
        ProtocolHandler* get_protocol() const { return protocol; }
    private:
        /* Member variables/attributes */
        Buffer<IBS> inbound;
        Buffer<OBS> outbound;
        ProtocolHandler *protocol;
        boost::asio::ip::tcp::socket tcp_socket;

        /* Member functions/methods */
        Endpoint(boost::asio::io_service &s);

        Endpoint();
        Endpoint(const Endpoint &e);
        Endpoint& operator= (const Endpoint &e);

        /*
         * Asynchronous operations - registers the handlers/callbacks with
         * the io_service which are executed (the handlers) when a read and/or
         * write has been completed on our behalf by the OS.
         */
        void async_recv(char *ptr, size_t max);
        void async_send(const char *ptr, size_t max);

        /*
         * Handler executed by io_service when read operation is performed.
         * e denotes an error code should the read have failed
         * b contains the number of bytes transferred
         */
        void
        raw_recv_callback(const boost::system::error_code& e, size_t b);

        /*
         * Handler executed by io_service when write operation is performed.
         * e denotes an error code should the write have failed
         * b contains the number of bytes transferred
         */
        void
        raw_send_callback(const boost::system::error_code& e, size_t b);

        /* Friend class declarations */
        friend class ProtocolHandler; // Needs access to inbound and outbound
};

} // net
} // oodles

#endif

