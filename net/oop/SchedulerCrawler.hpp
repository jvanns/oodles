#ifndef OODLES_NET_OOP_DIALECT_SCHEDULERCRAWLER_HPP
#define OODLES_NET_OOP_DIALECT_SCHEDULERCRAWLER_HPP

// oodles
#include "Protocol.hpp"
#include "Messages.hpp"
#include "common/Exceptions.hpp"

// STL
#include <vector>

namespace oodles {

namespace sched {

class Crawler; // Forward declaraton for SchedulerCrawler
class Scheduler; // Forward declaraton for SchedulerCrawler
   
} // sched
   
namespace net {
namespace oop {
namespace dialect {

class SchedulerCrawler : public ProtocolDialect
{
    public:
        /* Member functions/methods */
        SchedulerCrawler();

        /* Overrides the virtual method in ProtocolDialect translating
         * the incoming message subset into the known dialect between
         * the two components, Scheduler and Crawler.
         */
        void translate();

        /*
         * A crawler initiates the conversation by sending a RegisterCrawler
         * message to the Scheduler. It does so using this public method.
         */
        void register_crawler(const std::string &name, uint16_t cores);

        /*
         * The scheduler will eventually respond, after a schedule run,
         * to a RegisterCrawler message by sending URLs to the new crawler,
         * on an existing one, through this method. It passes to it all
         * URLs it scheduled the crawler to er, crawl.
         */
        void begin_crawl(const std::vector<url::URL*> &urls);
    private:
        /* Member functions/methods */
        void send(Message *m);
        Protocol& protocol() const;
        sched::Scheduler& scheduler() const;
        
        /*
         * Message handling methods
         */
        void continue_dialog(const Message *m) throw(DialectError);
        void continue_dialog(const RegisterCrawler &m);
        void continue_dialog(const BeginCrawl &m);
        void continue_dialog(const EndCrawl &m);
        
        /* Internal Data Structures */
        enum {
            Inbound = 0,
            Outbound = 1
        };

        /* Member variables/attributes */
        bool initiator;
        id_t context[2];
        static const id_t message_subset[];
};
   
} // dialect
} // oop
} // net
} // oodles

#endif
