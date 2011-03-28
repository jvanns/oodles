#ifndef OODLES_SCHED_OOP_SESSION_HPP
#define OODLES_SCHED_OOP_SESSION_HPP

// oodles
#include "DeferredUpdate.hpp"

#include "common/Exceptions.hpp"
#include "utility/Subscriber.hpp"

#include "net/oop/Session.hpp"
#include "net/oop/Messages.hpp"

// STL
#include <map>
#include <vector>

// Boost.thread
#include <boost/thread/mutex.hpp>

namespace oodles {
namespace sched {

class Context; // Forward declaraton for Session
   
namespace oop {

typedef net::oop::Message Message;
typedef net::oop::EndCrawl EndCrawl;
typedef net::oop::BeginCrawl BeginCrawl;
typedef net::oop::RegisterCrawler RegisterCrawler;

class Session : public net::oop::Session
{
    public:
        /* Dependent typedefs */
        typedef sched::Deferable::key_t key_t;
        
        /* Member functions/methods */
        Session();
        void handle_message(Message *m);

        /*
         * The scheduler will eventually respond, after a schedule run,
         * to a RegisterCrawler message by sending URLs to the new crawler,
         * on an existing one, through this method. It passes to it all
         * URLs it scheduled the crawler to er, crawl.
         */
        void begin_crawl(const std::vector<url::URL*> &urls);
    private:
        /* Internal Data Structures */
        class GarbageCollector : public event::Subscriber
        {
            public:
                /* Member functions/methods */
                
                /*
                 * Receives instruction from the publisher that it is done
                 * with a pointer passed to it and it can now be deleted at
                 * the sessions leisure.
                 */
                void receive(const event::Event::Ref e);
                void trash(Message *m, Session::key_t k);
            private:
                /* Member variables/attributes */
                boost::mutex mutex;
                std::map<Session::key_t, Message*> garbage;
        };
        
        /* Member functions/methods */
        void send(Message *m);
        Context& context() const;
        Scheduler& scheduler() const;
        
        /*
         * Message handling methods (inbound)
         */
        key_t continue_dialog(const Message *m) throw (net::DialogError);
        key_t continue_dialog(const RegisterCrawler &m);
        key_t continue_dialog(const EndCrawl &m);
        
        /* Internal Data Structures */
        enum {
            Inbound = 0,
            Outbound = 1
        };

        /* Member variables/attributes */
        id_t msg_context[2];
        GarbageCollector garbage;
        static const id_t message_subset[];
};
 
} // oop
} // sched
} // oodles

#endif
