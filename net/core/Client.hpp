#ifndef OODLES_NET_CLIENT_HPP
#define OODLES_NET_CLIENT_HPP

// oodles
#include "Endpoint.hpp"
#include "common/Exceptions.hpp"

// STL
#include <utility> // For std::pair

namespace oodles {

class Dispatcher; // Forward declaration for Client

namespace net {

class HandlerCreator; // Forward declaration for Client

class Client
{
    public:
        /* Member functions/methods */
        Client(Dispatcher &d, const HandlerCreator &c);

        void start(const std::string &service) throw (InvalidService);
        void stop();
    private:
        /* Member variables/attributes */
        Dispatcher &dispatcher;
        const HandlerCreator &handler_creator;
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
         * Upon establishing a successful connection to the resolved
         * service, we can detach this client so it becomes independently
         * managed.
         */
        void detach_client(Endpoint::Connection c) const;

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
