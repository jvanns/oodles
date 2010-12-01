#ifndef OODLES_CRAWL_CONTEXT_HPP
#define OODLES_CRAWL_CONTEXT_HPP

// oodles
#include "Crawler.hpp"
#include "utility/Proactor.hpp"
#include "net/core/Client.hpp"
#include "net/oop/SchedulerCrawler.hpp"

namespace oodles {
namespace crawl {

class Context
{
    public:
        /* Member functions/methods */
        Context(const std::string &name, uint16_t cores);

        void stop_crawling();
        void start_crawling(const std::string &service);
    private:
        /* Dependent typedefs */
        typedef net::Client Client;
        typedef net::oop::Protocol OOP;
        typedef net::oop::dialect::SchedulerCrawler SchedulerCrawler;
        
        /* Member variables/attributes */

        /*
         * Asynchronous task dispatcher
         */
        Proactor dispatcher;
        
        /*
         * Crawler layer
         */
        Crawler crawler;

        /*
         * Network layers
         */
        const net::Protocol<OOP, SchedulerCrawler> creator;
        Client client;
};

} // crawl
} // oodles

#endif

