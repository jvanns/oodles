#ifndef OODLES_NET_CLIENT_HPP
#define OODLES_NET_CLIENT_HPP

// oodles
#include "Endpoint.hpp"
#include "utility/Linker.hpp"

namespace oodles {

class Dispatcher; // Forward declaration for Client

namespace net {

class ProtocolCreator; // Forward declaration for Client

class Client : public Linker
{
    public:
        /* Member functions/methods */
        Client(Dispatcher &d, const ProtocolCreator &c);

        void start(const std::string &service);
        void stop();
    private:
        /* Member variables/attributes */
        Dispatcher &dispatcher;
        const ProtocolCreator &protocol_creator;
        boost::asio::ip::tcp::resolver resolver;

        /* Member functions/methods */
        Client(const Client &s);
        Client& operator= (const Client &s);

        /*
         * Asynchronous operations - registers the handlers/callbacks with
         * the io_service which are executed (the handlers) when a connect or
         * resolution has been completed on our behalf by the OS.
         */
        void async_resolve(const std::string &service);
        void async_connect(boost::asio::ip::tcp::resolver::iterator &i);

        /*
         * Handler executed by io_service when accept operation is performed.
         * e denotes an error code should the accept have failed
         * i is the first of n entries the resolver returned upon success
         */
        void resolver_callback(const boost::system::error_code &e,
                               boost::asio::ip::tcp::resolver::iterator i);

        /*
         * Handler executed by io_service when connect operation is performed.
         * e denotes an error code should the connect have failed
         * c is the connected endpoint upon success
         */
        void connect_callback(const boost::system::error_code &e,
                              boost::asio::ip::tcp::resolver::iterator i,
                              Endpoint::Connection c);
};

} // net
} // oodles

#endif
