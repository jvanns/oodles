// oodles
#include "Server.hpp"
#include "HandlerCreator.hpp"
#include "utility/Dispatcher.hpp"

// Boost.bind (TR1 is incompatible)
#include <boost/bind.hpp>

// Boost.asio
#include <boost/asio/placeholders.hpp>

// libc
#include <limits.h> // For USHRT_MAX
#include <stdlib.h> // For atoi()

// STL
using std::string;

// Boost.bind
using boost::bind;

// Boost.system
using boost::system::error_code;

// Boost.asio
using boost::asio::ip::address;
typedef boost::asio::ip::tcp::endpoint endpoint;
typedef boost::asio::ip::tcp::acceptor acceptor;
typedef boost::asio::ip::tcp::resolver resolver;

/*
 * Parses the ip:port formatted string and returns a Boost.asio endpoint
 */
static
endpoint
endpoint_from_service(const string &service)
throw (oodles::net::InvalidService)
{
    const string::size_type split = service.find_first_of(':');

    if (split == string::npos)
        throw oodles::net::InvalidService("endpoint_from_service",
                                          0,
                                          "Service not in ip:port format.");

    const address a(address::from_string(service.substr(0, split)));
    const int p = atoi(service.substr(split + 1, service.size()).c_str());

    if (p > USHRT_MAX)
        throw oodles::net::InvalidService("endpoint_from_service",
                                          0,
                                          "Port number must be less than %u.",
                                          USHRT_MAX);

    return endpoint(a, p);
}

namespace oodles {
namespace net {

// Save my fingers!
namespace placeholders = boost::asio::placeholders;

Server::Server(Dispatcher &d, const HandlerCreator &c) :
    reverse_lookup(false),
    dispatcher(d),
    handler_creator(c),
    acceptor(d.io_service()),
    resolver(d.io_service())
{
}

void
Server::start(const string &service)
throw (InvalidService)
{
    const endpoint e(endpoint_from_service(service));
    error_code c;

    /*
     * Always open the socket before even setting the options
     */
    acceptor.open(e.protocol());

    acceptor.set_option(acceptor::reuse_address(true));
    acceptor.bind(e, c);

    if (c)
        throw InvalidService("Server::start",
                             errno,
                             "Failed to bind to given service; %s.",
                             service.c_str());

    acceptor.listen();
    async_accept();
}

void
Server::stop()
{
    resolver.cancel();
    acceptor.close();
}

void
Server::async_accept()
{
    Endpoint::Connection c(Endpoint::create(dispatcher));

    acceptor.async_accept(c->socket(), bind(&Server::acceptor_callback,
                                            this,
                                            placeholders::error,
                                            c));
}

void
Server::async_resolve(Endpoint::Connection c)
{
    const endpoint &e = c->socket().remote_endpoint();

    resolver.async_resolve(e, bind(&Server::resolver_callback,
                                   this,
                                   placeholders::error,
                                   placeholders::iterator));
}

void
Server::detatch_client(Endpoint::Connection c) const
{
    ProtocolHandler *p = handler_creator.create_protocol();
    SessionHandler *s = handler_creator.create_session();
    CallerContext &cc = handler_creator.caller_context();
    
    c->set_protocol(p);
    c->set_session(s);
    
    c->start(cc);
}

void
Server::acceptor_callback(const error_code &e, Endpoint::Connection c)
{
    if (!e) {
        if (reverse_lookup)
            async_resolve(c);
        else
            detatch_client(c);

        async_accept();
    } else {
        if (e != boost::asio::error::operation_aborted)
            stop();
    }
}

void
Server::resolver_callback(const error_code &e, resolver::iterator i)
{
    if (!e) {
    } else {
        if (e != boost::asio::error::operation_aborted)
            stop();
    }
}

} // net
} // oodles
