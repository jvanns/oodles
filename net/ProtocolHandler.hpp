#ifndef OODLES_NET_PROTOCOLHANDLER_HPP
#define OODLES_NET_PROTOCOLHANDLER_HPP

// oodles
#include "Endpoint.hpp"

// STL
#include <string>

namespace oodles {
namespace net {

class ProtocolHandler
{
    public:
        /* Member functions/methods */
        virtual ~ProtocolHandler();

        /*
         * Pair endpoint with protocol handler
         */
        void set_endpoint(Endpoint::Connection e);

        /*
         * Begin a transfer. The (outbound) buffer is prepared
         * for a write operation and the buffer area and size
         * allocated is passed to message2buffer(), called by
         * this method.
         */
        void transfer_data();

        /*
         * Initial method called by a newly connected endpoint
         */
        virtual void start() = 0;

        /*
         * Return the protocol name/identifier such as
         * http, smtp, ftp, imap, pop3, etc.
         */
        virtual std::string name() const = 0;

        /*
         * Here our associated Endpoint informs us of how many bytes
         * were sent out of our request to send n messages from the buffer.
         */
        virtual void bytes_transferred(size_t n) = 0;

        /*
         * Implementation defined method of putting a message onto
         * the raw network buffer. This method is called from the
         * non-virtual call transfer_data().
         *
         * You should return the number of bytes you used from the
         * buffer which should of course be <= max. If the size of
         * your message exceeds max then, from bytes_transferred(),
         * you must call transfer_data() until all data is sent.
         */
        virtual size_t message2buffer(char *& buffer, size_t max) = 0;

        /*
         * Read and process (block until finished with data) a message.
         * Return the remainder of any unused bytes which will continue
         * to be present in the next call to buffer2message. Upon return
         * the buffer data will be overwritten by subsequent raw socket
         * recv()s through the Endpoint, endpoint.
         */
        virtual size_t buffer2message(const char *buffer, size_t max) = 0;
    protected:
        /* Member functions/methods */
        ProtocolHandler() {}
        void finish() { endpoint->close(); }
    private:
        /* Member variables/attributes */
        Endpoint::Connection endpoint;

        /* Member functions/methods */
        ProtocolHandler(const ProtocolHandler &source);
        ProtocolHandler& operator= (const ProtocolHandler &source);
};

} // net
} // oodles

#endif
