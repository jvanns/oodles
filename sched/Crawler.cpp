// oodles
#include "Crawler.hpp"
#include "url/URL.hpp"
#include "net/oop/Protocol.hpp"

// STL
using std::string;

namespace oodles {
namespace sched {

Crawler::Crawler(const string &name) : cores(1), name(name)
{
    work_unit.reserve(max_unit_size());
}

void
Crawler::begin_crawl()
{
    net::ProtocolDialect *dialect = endpoint->get_protocol()->get_dialect();
    // TODO: Use (yet to be written) real Crawler/Scheduler dialect class
}

Crawler::unit_t
Crawler::add_url(const url::URL &url)
{
    work_unit.push_back(&url);

#ifdef DEBUG_SCHED
    std::cerr << '[' << name << "]: "
              << *(work_unit.back())
              << " assigned\n";
#endif

    return assigned();
}

bool
RankCrawler::operator() (const Crawler *lhs, const Crawler *rhs) const
{
    /*
     * First, be sure to check that the Crawlers have a valid Endpoint!
     */
    if (lhs->online() && rhs->online()) {
        if (lhs->assigned() == rhs->assigned())
            return false; // Maintain a stable queue

        /*
         * Always prefer Crawlers with less work at the front of the queue
         */
        return lhs->assigned() > rhs->assigned();
    }

    /*
     * If neither are online, then...
     */
    if (!lhs->online() && !rhs->online())
        return false; // Again, maintain a stable queue

    if (!lhs->online())
        return true;

    return false;
}

} // sched
} // oodles
