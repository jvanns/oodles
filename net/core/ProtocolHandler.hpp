#ifndef OODLES_NET_PROTOCOLHANDLER_HPP
#define OODLES_NET_PROTOCOLHANDLER_HPP

// oodles
#include "Endpoint.hpp"

// STL
#include <string>
#include <iostream>

namespace oodles {
namespace net {

class ProtocolHandler
{
    public:
        /* Member functions/methods */
        ProtocolHandler() {}
        virtual ~ProtocolHandler() {}

        /*
         * Pair endpoint with protocol handler
         */
        void set_endpoint(Endpoint::Connection e);
        Endpoint::Connection get_endpoint() const { return endpoint; }

        /*
         * Start any data transfer
         */
        void start();

        /*
         * Begin a transfer. The (outbound) buffer is prepared
         * for a write operation and the buffer area and size
         * allocated is passed to message2buffer(), called by
         * this method.
         *
         * The counter-par to transfer_data() is of course,
         * receive_data(). Similarly it prepares the inbound
         * buffer for a read operation and upon receiving any
         * data will call buffer2message.
         */
        void receive_data();
        void transfer_data(size_t pending = 0);
        
        /*
         * Called by Endpoint, this registers, with the Dispatcher,
         * a callback (handle_messages in a derived SessionHandler)
         * to be executed to handle any messages now ready for processing
         * after buffer2message has been exhuasted by the async_recv()
         * callback.
         */
        void handle_messages(Dispatcher &d) const;
        
        /*
         * Virtual method for printing protocol specific
         * statistics/metrics such as messages inbound,
         * maximum message size, message throughput etc.
         */
        virtual void print_metrics(std::ostream *s) const;

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
        virtual size_t message2buffer(char *buffer, size_t max) = 0;

        /*
         * Read and process (block until finished with data) a message.
         * Return the number of used bytes so the endpoint can preserve
         * the remainder which will presented once again to this method
         * upon the next receipt of incoming data.
         */
        virtual size_t buffer2message(const char *buffer, size_t max) = 0;
    protected:
        /* Member functions/methods */
        void stop() { endpoint->stop(); }
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
