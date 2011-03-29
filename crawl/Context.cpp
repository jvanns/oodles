// oodles
#include "Context.hpp"
#include "utility/Linker.hpp"

// STL
using std::string;

namespace oodles {
namespace crawl {

// NetContext
Context::NetContext::NetContext(Context *c) : context(c) {}

void
Context::NetContext::start(net::SessionHandler &s)
{
    oop::Session *session = static_cast<oop::Session*>(&s);
    const Link l(context, session);

    session->register_crawler();
}

// Context
Context::Context(const string &name, uint16_t cores) :
    crawler(name, cores),
    net_context(this),
    creator(net_context),
    client(dispatcher, creator)
{
}

void
Context::stop_crawling()
{
    dispatcher.stop();
}

void
Context::start_crawling(const string &service)
{
    client.start(service);
    dispatcher.wait();
}

} // crawl
} // oodles
