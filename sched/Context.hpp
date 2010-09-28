#ifndef OODLES_SCHED_CONTEXT_HPP
#define OODLES_SCHED_CONTEXT_HPP

// oodles
#include "sched/Crawler.hpp"
#include "sched/Scheduler.hpp"

#include "net/core/Server.hpp"
#include "net/oop/SchedulerCrawler.hpp"

#include "utility/Proactor.hpp"
#include "utility/BreadCrumbTrail.hpp"

// STL
#include <map>

namespace oodles {
namespace sched {

class Context
{
    public:
        /* Member functions/methods */
        Context();
        
        void start_crawling(int interval = 1);
        void seed_scheduler(const std::string &url);
        void start_server(const std::string &service);
        Crawler& create_crawler(const std::string &name, uint16_t cores);
    private:
        /* Dependent typedefs */
        typedef net::Server Server;
        typedef net::oop::Protocol OOP;
        typedef net::oop::dialect::SchedulerCrawler SchedulerCrawler;
        
        /* Member variables/attributes */

        /*
         * Asynchronous task dispatcher
         */
        Proactor proactor;

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
};

} // sched
} // oodles

#endif
