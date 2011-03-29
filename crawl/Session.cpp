// oodles
#include "Session.hpp"
#include "Context.hpp"
#include "Crawler.hpp"

// STL
using std::list;
using std::string;
using std::vector;
using std::make_pair;
using std::exception;

namespace oodles {
namespace crawl {
namespace oop {

using net::DialogError;
using net::oop::END_CRAWL;
using net::oop::INVALID_ID;
using net::oop::BEGIN_CRAWL;
using net::oop::REGISTER_CRAWLER;

/* This array holds the valid subset of messages understood by this dialog */
const id_t Session::message_subset[] = {
    REGISTER_CRAWLER,
    BEGIN_CRAWL,
    END_CRAWL
};

// Session
Session::Session()
{
    msg_context[Inbound] = msg_context[Outbound] = INVALID_ID;
}

void
Session::handle_message(Message *m)
{
    try {
        continue_dialog(m);
        delete m;
    } catch (const exception &e) {
        delete m;
        throw;
    }
}

void
Session::register_crawler()
{
    RegisterCrawler *m = new RegisterCrawler;
    m->cores = crawler().cores();
    m->name = crawler().id();
    
    send(m); // Non-blocking
}

void
Session::end_crawl(const list<url::URL> &urls)
{
    EndCrawl *m = new EndCrawl;
    list<url::URL>::const_iterator i = urls.begin(), j = urls.end();
    
    while (i != j) {
        m->scheduled_urls.push_back(make_pair(i->page_id(), true)); // FIXME
        ++i;
    }

    send(m);
}

void
Session::send(Message *m)
{
    msg_context[Outbound] = m->id();
    push_message(m);
}

inline
Context&
Session::context() const
{   
    return *static_cast<Context*>(coupling->complement_of(*this));
}

inline
Crawler&
Session::crawler() const
{   
    return context().get_crawler();
}

void
Session::continue_dialog(const Message *m)
throw (DialogError)
{
    /*
     * Verify the context is valid based on this *incoming* message,
     * the previous outbound message and previous inbound message.
     */
    switch (m->id()) {
        /* Crawler Inbound */
        case BEGIN_CRAWL:
            if (msg_context[Inbound] != INVALID_ID &&
                msg_context[Inbound] != BEGIN_CRAWL)
                throw DialogError("Session::continue_dialog",
                                   0,
                                   "Invalid inbound context: P=#%d, C=#%d.",
                                   msg_context[Inbound], m->id());
            
            if (msg_context[Outbound] != END_CRAWL &&
                msg_context[Outbound] != REGISTER_CRAWLER)
                throw DialogError("Session::continue_dialog",
                                   0,
                                   "Invalid outbound context: P=#%d, C=#%d.",
                                   msg_context[Outbound], m->id());
            
            continue_dialog(static_cast<const BeginCrawl&>(*m));
            break;
        default:
            throw DialogError("Session::continue_dialog",
                               0,
                               "Received unexpected OOP message #%d.",
                               m->id());
    }
    
    msg_context[Inbound] = m->id();
}

void
Session::continue_dialog(const BeginCrawl &m)
{
    /*
     * Instruct the endpoint-owning Crawler to begin crawling
     * all URLs given in m as scheduled by the Scheduler that
     * sent this message.
     */
    list<const url::URL*>::const_iterator i = m.urls.begin(), j = m.urls.end();

    while (i != j) {
        const url::URL &u = *(*i);
        crawler().fetch(u);
    }
}

} // oop
} // crawl
} // oodles
