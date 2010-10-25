// oodles
#include "Crawler.hpp"
#include "url/URL.hpp"
#include "net/oop/SchedulerCrawler.hpp"

// STL
using std::string;
using std::vector;
using std::lower_bound;

namespace oodles {
namespace sched {

Crawler::Crawler(const string &name, uint16_t cores) : cores(cores), name(name)
{
    work_unit.reserve(max_unit_size());
}

void
Crawler::begin_crawl()
{
    using net::oop::dialect::SchedulerCrawler;

    SchedulerCrawler &dialect = *(endpoint->get_protocol()->get_dialect());
    dialect.begin_crawl(work_unit); // All scheduled URLs will be sent
}

Crawler::unit_t
Crawler::add_url(url::URL &url)
{
    work_unit.insert(lower_bound(work_unit.begin(),
                                 work_unit.end(),
                                 &url), &url);
    return assigned();
}

Crawler::unit_t
Crawler::remove_url(url::URL &url)
{
    vector<url::URL*>::iterator i = lower_bound(work_unit.begin(),
                                                work_unit.end(),
                                                &url);

    assert(i != work_unit.end() && *i == &url);
    work_unit.erase(i);

    return assigned();
}

void
Crawler::set_endpoint(net::Endpoint::Connection e)
{
    assert(!endpoint);
    endpoint = e;
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
