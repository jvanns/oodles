#ifndef OODLES_NET_ENDPOINT_HPP
#define OODLES_NET_ENDPOINT_HPP

// Boost.shared
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
/* NOTE: When Boost and TR1 shared_ptr are compatible prefer <tr1/memory> */

// Boost.asio
#include <boost/asio/ip/tcp.hpp>

// STL 
#include <tr1/array>

// libc
#include <stdint.h> // For uint16_t

namespace oodles {
namespace net {

static const uint16_t OBS =  /* Outbound buffer size */
#ifdef OUTBOUND_BUFFER_SIZE
       OUTBOUND_BUFFER_SIZE <= UINT16_MAX
       ? OUTBOUND_BUFFER_SIZE
       : UINT16_MAX;
#else
       8192;
#endif

static const uint16_t IBS = /* Inbound buffer size */
#ifdef INBOUND_BUFFER_SIZE
       INBOUND_BUFFER_SIZE <= UINT16_MAX
       ? INBOUND_BUFFER_SIZE
       : UINT16_MAX;
#else
       8192;
#endif

class ProtocolHandler; // Forward declaration for Endpoint

class Endpoint : public boost::enable_shared_from_this<Endpoint>
{
    public:
        /* Dependent typedefs */
        typedef boost::shared_ptr<Endpoint> Connection;
        typedef boost::shared_ptr<ProtocolHandler> Protocol;

        /* Member functions/methods */
        ~Endpoint();

        static
        Endpoint*
        create(boost::asio::io_service &s)
        {
            return new Endpoint(s);
        }

        boost::asio::ip::tcp::socket& socket() { return tcp_socket; }

        void stop(); // Close socket cancelling pending handlers
        void start(); // Must be called to register reads/writes
        void set_protocol(Protocol p); // Pair protocol with handler
    private:
        /* Dependent typedefs */
        typedef char byte_t;

        /* Internal Data Structures */
        template<size_t max> struct Buffer
        {
            bool locked;
            size_t offset;
            std::tr1::array<byte_t, max> buffer;

            Buffer() : locked(false), offset(0) {}

            byte_t* data() { return buffer.begin(); }
            const byte_t* data() const { return buffer.begin(); }

            bool empty() const { return offset == 0; }
            bool full() const { return offset == size(); }

            size_t size() const { return buffer.size(); }
            void consume(size_t n) { std::rotate(data(),
                                                 data() + n,
                                                 data() + size()); }
        };

        /* Member variables/attributes */
        Protocol protocol;
        Buffer<IBS> inbound;
        Buffer<OBS> outbound;
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
        void async_recv();
        void async_send();

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

        /*
         * The reference *buffer will be set to the outbound buffer and on
         * function return, the current capacity of that buffer. This  method
         * provides, to it's ProtocolHandler, access to the internal outbound
         * buffer ready for using it directly as the memory area for creating
         * messages - avoiding any unnecessary object copies if the message can
         * fit in the buffer.
         */
        size_t prepare_outbound_buffer(byte_t *&buffer);
        void set_outbound_offset(size_t offset) { outbound.offset = offset; }

        /* Friend class declarations */
        friend class ProtocolHandler; // ProtocolHandler will call the above
};

} // net
} // oodles

#endif

