#ifndef OODLES_SCHED_CONTEXT_HPP
#define OODLES_SCHED_CONTEXT_HPP

// oodles
#include "Crawler.hpp"
#include "Scheduler.hpp"

#include "net/core/Server.hpp"
#include "net/oop/SchedulerCrawler.hpp"

#include "utility/Dispatcher.hpp"
#include "utility/BreadCrumbTrail.hpp"

// STL
#include <map>
#include <iostream>

namespace oodles {
namespace sched {

class Context
{
    public:
        /* Member functions/methods */
        Context();
        
        void seed_scheduler(const std::string &url);
        void start_server(const std::string &service);
        Crawler& create_crawler(const std::string &name, uint16_t cores);

        void stop_crawling();
        void start_crawling(std::ostream *dot_stream = NULL, int interval = 1);
    private:
        /* Dependent typedefs */
        typedef net::Server Server;
        typedef net::oop::Protocol OOP;
        typedef net::oop::dialect::SchedulerCrawler SchedulerCrawler;
        
        /* Member variables/attributes */

        /*
         * Asynchronous task dispatcher
         */
        Dispatcher dispatcher;

        /*
         * Network layers
         */
        Server server;
        const net::Protocol<OOP, SchedulerCrawler> creator;

        /*
         * Scheduler layer
         */
        Scheduler scheduler;
        BreadCrumbTrail trail;
        std::map<std::string, Crawler> crawlers;

        /* Friend class declarations */
        friend class Scheduler; // Scheduler wants to access dispatcher
};

} // sched
} // oodles

#endif

