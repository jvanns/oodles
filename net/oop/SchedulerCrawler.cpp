// oodles
#include "Messages.hpp"
#include "SchedulerCrawler.hpp"

// STL
using std::string;
using std::vector;
using std::exception;

namespace oodles {
namespace net {
namespace oop {
namespace dialect {

/* This array holds the valid subset of messages understood by this dialect */
const id_t SchedulerCrawler::message_subset[] = {
    REGISTER_CRAWLER,
    BEGIN_CRAWL,
    END_CRAWL
};

SchedulerCrawler::SchedulerCrawler() : initiator(false)
{
    context[Inbound] = context[Outbound] = INVALID_ID;
}

void
SchedulerCrawler::translate()
{
    Message *m = protocol().pop_message();
    
    try {
        while (m) {
            continue_dialog(m);
            m = protocol().pop_message();
        }
    } catch (const exception &e) {
        delete m;
    }
}

void
SchedulerCrawler::register_crawler(const string &name, uint16_t cores)
{
    RegisterCrawler *m = new RegisterCrawler;
    m->cores = cores;
    m->name = name;

    initiator = true;
    
    send(m); // Non-blocking
}

void
SchedulerCrawler::begin_crawl(const vector<url::URL*> &urls)
{
    BeginCrawl *m = new BeginCrawl;
    vector<url::URL*>::const_iterator i = urls.begin(), j = urls.end();
    
    while (i != j) {
        url::URL &u = *(*i);
        m->urls[u.domain_id()].push_back(BeginCrawl::URL(u.page_id(),
                                                         u.to_string()));
        ++i;
    }

    send(m);
}

void
SchedulerCrawler::send(Message *m)
{
    context[Outbound] = m->id();
    protocol().push_message(m);
}

Protocol&
SchedulerCrawler::protocol() const
{
    assert(handler != NULL);
    return *handler;
}

void
SchedulerCrawler::continue_dialog(const Message *m)
throw (DialectError)
{
    /*
     * Verify the context is valid based on this *incoming* message,
     * the previous outbound message and previous inbound message.
     */
    switch (m->id()) {
        /* Crawler Inbound */
        case BEGIN_CRAWL:
            if (!initiator)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Conversation initialised by Scheduler!?");
                
            if (context[Inbound] != INVALID_ID ||
                context[Inbound] != BEGIN_CRAWL)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());
            
            if (context[Outbound] != END_CRAWL ||
                context[Outbound] != REGISTER_CRAWLER)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());
            
            continue_dialog(static_cast<const BeginCrawl&>(*m));
            break;
        /* Scheduler Inbound */
        case REGISTER_CRAWLER:
           if (initiator)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Conversation initialised by Scheduler!?");
            
            if (context[Inbound] != INVALID_ID)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());
            
            if (context[Outbound] != INVALID_ID)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());

            continue_dialog(static_cast<const RegisterCrawler&>(*m));
            break;
        case END_CRAWL:
           if (initiator)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Conversation initialised by Scheduler!?");
            
            if (context[Inbound] != REGISTER_CRAWLER)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());
            
            if (context[Outbound] != BEGIN_CRAWL)
                throw DialectError("SchedulerCrawler::continue_dialog",
                                   0,
                                   "Invalid dialog context at message #%d.",
                                   m->id());

            continue_dialog(static_cast<const EndCrawl&>(*m));
            break;
        default:
            throw DialectError("SchedulerCrawler::continue_dialog",
                               0,
                               "Received unexpected OOP message #%d.",
                               m->id());
    }
    
    context[Inbound] = m->id();
    delete m;
}

void
SchedulerCrawler::continue_dialog(const RegisterCrawler &m)
{
    /*
     * Register, with the sched/Scheduler, a new sched/Crawler
     * based on this incoming data (name, cores etc.)
     */
}

void
SchedulerCrawler::continue_dialog(const BeginCrawl &m)
{
    /*
     * Instruct the endpoint-owning Crawler to begin crawling
     * all URLs given in m as scheduled by the Scheduler that
     * sent this message.
     */
}

void
SchedulerCrawler::continue_dialog(const EndCrawl &m)
{
    /*
     * Update the URL-tree in the scheduler with the data
     * contained in the received message, m, as sent
     * by the Crawler.
     */
}

} // dialect
} // oop
} // net
} // oodles
