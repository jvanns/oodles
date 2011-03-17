// oodles
#include "Client.hpp"
#include "HandlerCreator.hpp"
#include "SessionHandler.hpp"
#include "ProtocolHandler.hpp"
#include "common/Exceptions.hpp"
#include "utility/Dispatcher.hpp"

// Boost.bind (TR1 is incompatible)
#include <boost/bind.hpp>

// Boost.asio
#include <boost/asio/placeholders.hpp>

// STL
using std::string;

// Boost.bind
using boost::bind;

// Boost.system
using boost::system::error_code;

// Boost.asio
using boost::asio::ip::address;
typedef boost::asio::ip::tcp::endpoint endpoint;
typedef boost::asio::ip::tcp::resolver resolver;

/*
 * Parses the ip:port formatted string and returns a Boost.asio query
 */
static
resolver::query
query_from_service(const string &service)
throw (oodles::net::InvalidService)
{
    const string::size_type split = service.find_first_of(':');

    if (split == string::npos)
        throw oodles::net::InvalidService("query_from_service",
                                          0,
                                          "Service not in ip:port format.");

    const string h(service.substr(0, split)); // Host name
    const string p(service.substr(split + 1, service.size()).c_str()); // Port

    if (atoi(p.c_str()) > USHRT_MAX)
        throw oodles::net::InvalidService("query_from_service",
                                          0,
                                          "Port number must be less than %u.",
                                          USHRT_MAX);

    return resolver::query(h, p);
}

namespace oodles {
namespace net {

// Save my fingers!
namespace placeholders = boost::asio::placeholders;

Client::Client(Dispatcher &d, const HandlerCreator &c) :
    dispatcher(d),
    handler_creator(c),
    resolver(d.io_service())
{
}

void
Client::start(const string &service, SessionHandler &s)
{
    async_resolve(PendingSession(service, s));
}

void
Client::stop()
{
    resolver.cancel();
}

void
Client::async_resolve(PendingSession ps)
{
    const resolver::query q(query_from_service(ps.first));

    resolver.async_resolve(q, bind(&Client::resolver_callback,
                                   this,
                                   placeholders::error,
                                   placeholders::iterator,
                                   ps));
}

void
Client::async_connect(PendingSession ps,  resolver::iterator &i)
{
    Endpoint::Connection c(Endpoint::create(dispatcher));

    c->set_session(&ps.second);
    c->socket().async_connect(*i, bind(&Client::connect_callback,
                                       this,
                                       placeholders::error,
                                       i++,
                                       ps));
}

void
Client::detach_client(Endpoint::Connection c) const
{
    ProtocolHandler *p = handler_creator.create_protocol();
   
    c->set_protocol(p);
    c->start();
}

void
Client::resolver_callback(const error_code &e,
                          resolver::iterator &i,
                          PendingSession ps)
{
    if (!e) {
        async_connect(ps, i);
    } else {
       throw DNSFailure("Client::resolver_callback",
                        e.value(),
                        "Failed to resolve '%s'.", ps.first.c_str());
    }
}

void
Client::connect_callback(const error_code &e,
                         resolver::iterator i,
                         PendingSession ps)
{
    if (!e) {
        detach_client(ps.second.get_endpoint());
    } else {
        static const resolver::iterator end;

        if (i == end)
            throw OpenError("Client::connect_callback",
                             0,
                             "Failed to connect to (resolved) service.");

        ps.second.get_endpoint()->stop();
        async_connect(ps, i);
    }
}

} // net
} // oodles
