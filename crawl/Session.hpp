#ifndef OODLES_CRAWL_OOP_SESSION_HPP
#define OODLES_CRAWL_OOP_SESSION_HPP

// oodles
#include "common/Exceptions.hpp"

#include "net/oop/Session.hpp"
#include "net/oop/Messages.hpp"

// STL
#include <list>

namespace oodles {
namespace crawl {

class Crawler; // Forward declaration for Session
class Context; // Forward declaration for Session

namespace oop {

typedef net::oop::Message Message;
typedef net::oop::EndCrawl EndCrawl;
typedef net::oop::BeginCrawl BeginCrawl;
typedef net::oop::RegisterCrawler RegisterCrawler;

class Session : public net::oop::Session
{
    public:
        /* Member functions/methods */
        Session();
        void handle_message(Message *m);

        /*
         * A crawler initiates the conversation by sending a RegisterCrawler
         * message to the Scheduler. It does so using this public method. An
         * instance of this class, Session, then keeps a pointer to
         * the crawler Context now via the Linker system.
         */
        void register_crawler();

        /*
         * The crawler will return a list of URLs to the scheduler once it
         * has (attmpted) to crawl them. The state (success/failure) of the
         * URL is sent along with it. Generally the list of URLs provided here
         * will be from the same domain.
         */
        void end_crawl(const std::list<url::URL> &urls);
    private:
        /* Member functions/methods */
        void send(Message *m);
        Context& context() const;
        Crawler& crawler() const;
        
        /*
         * Message handling methods (inbound)
         */
        void continue_dialog(const Message *m) throw (net::DialogError);
        void continue_dialog(const BeginCrawl &m);
        
        /* Internal Data Structures */
        enum {
            Inbound = 0,
            Outbound = 1
        };

        /* Member variables/attributes */
        id_t msg_context[2];
        static const id_t message_subset[];
};
   
} // oop
} // crawl
} // oodles

#endif
