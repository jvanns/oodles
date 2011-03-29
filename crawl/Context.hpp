#ifndef OODLES_CRAWL_CONTEXT_HPP
#define OODLES_CRAWL_CONTEXT_HPP

// oodles
#include "Session.hpp"
#include "Crawler.hpp"

#include "net/core/Client.hpp"
#include "net/oop/Protocol.hpp"
#include "net/core/HandlerCreator.hpp"

#include "utility/Linker.hpp"
#include "utility/Dispatcher.hpp"

namespace oodles {
namespace crawl {

class Context : public Linker
{
    public:
        /* Member functions/methods */
        Context(const std::string &name, uint16_t cores);

        Crawler& get_crawler() { return crawler; }

        void stop_crawling();
        void start_crawling(const std::string &service);
    private:
        /* Internal Data Structures */
        class NetContext : public net::CallerContext
        {
            public:
                NetContext(Context *c);
                void start(net::SessionHandler &s);
            private:
                Context *context;
        };
        
        /* Member variables/attributes */

        /*
         * Asynchronous task dispatcher
         */
        Dispatcher dispatcher;
        
        /*
         * Crawler layer
         */
        Crawler crawler;

        /*
         * Network layers
         */
        typedef net::Creator<net::oop::Protocol, oop::Session> Creator;
        NetContext net_context;
        const Creator creator;
        net::Client client;
};

} // crawl
} // oodles

#endif

