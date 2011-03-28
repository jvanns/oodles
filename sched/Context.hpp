#ifndef OODLES_SCHED_CONTEXT_HPP
#define OODLES_SCHED_CONTEXT_HPP

// oodles
#include "Session.hpp"
#include "Crawler.hpp"
#include "Scheduler.hpp"

#include "net/core/Server.hpp"
#include "net/oop/Protocol.hpp"
#include "net/core/HandlerCreator.hpp"

#include "utility/Linker.hpp"
#include "utility/Dispatcher.hpp"
#include "utility/BreadCrumbTrail.hpp"

// STL
#include <map>
#include <iostream>

namespace oodles {
namespace sched {

class Context : public Linker
{
    public:
        /* Member functions/methods */
        Context();
        
        Scheduler& get_scheduler() { return scheduler; }
        
        void seed_scheduler(const std::string &url);
        void start_server(const std::string &service);
        Crawler& create_crawler(const std::string &name, uint16_t cores);

        void stop_crawling();
        void start_crawling(std::ostream *dot_stream = NULL, int interval = 1);
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
         * Scheduler layer
         */
        Scheduler scheduler;
        BreadCrumbTrail trail;
        std::map<std::string, Crawler> crawlers;

        /*
         * Network layer
         */ 
        typedef net::Creator<net::oop::Protocol, oop::Session> Creator;
        NetContext net_context;
        const Creator creator;
        net::Server server;
};

} // sched
} // oodles

#endif

