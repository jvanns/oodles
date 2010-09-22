#ifndef OODLES_NET_SERVER_HPP
#define OODLES_NET_SERVER_HPP

// oodles
#include "Endpoint.hpp"
#include "utility/Linker.hpp"
#include "common/Exceptions.hpp"

namespace oodles {
namespace net {

class ProtocolCreator; // Forward declaration for Server

class Server : public Linker
{
    public:
        /* Member functions/methods */
        Server(boost::asio::io_service &s, const ProtocolCreator &c);

        void start(const std::string &service) throw (InvalidService);
        void stop();
    private:
        /* Member variables/attributes */
        bool reverse_lookup;
        const ProtocolCreator &protocol_creator;
        boost::asio::ip::tcp::acceptor acceptor;
        boost::asio::ip::tcp::resolver resolver;

        /* Member functions/methods */
        Server(const Server &s);
        Server& operator= (const Server &s);

        /*
         * Asynchronous operations - registers the handlers/callbacks with
         * the io_service which are executed (the handlers) when an accept or
         * resolution has been completed on our behalf by the OS.
         */
        void async_accept();
        void async_resolve(Endpoint::Connection c);
        void detatch_client(Endpoint::Connection c) const;

        /*
         * Handler executed by io_service when accept operation is performed.
         * e denotes an error code should the accept have failed
         * c is the connected endpoint upon success
         */
        void acceptor_callback(const boost::system::error_code &e,
                                          Endpoint::Connection c);

        /*
         * Handler executed by io_service when accept operation is performed.
         * e denotes an error code should the accept have failed
         * i is the first of n entries the resolver returned upon success
         */
        void resolver_callback(const boost::system::error_code &e,
                               boost::asio::ip::tcp::resolver::iterator i);
};

} // net
} // oodles

#endif
