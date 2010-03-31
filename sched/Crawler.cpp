// oodles
#include "Crawler.hpp"

namespace  oodles {
namespace sched {

Crawler::Crawler() : endpoint(NULL)
{
}

uint16_t
Crawler::add_url(const url::URL *url)
{
    work_unit.push_back(url);
    return unit_size();
}

bool
RankCrawler::operator() (const Crawler *lhs, const Crawler *rhs) const
{
    /*
     * First, be sure to check that the Crawler has a valid Endpoint!
     */
    if (!lhs->online())
        return false;

    /*
     * Always prefer Crawlers with less work at the front of the queue
     */
    return lhs->unit_size() < rhs->unit_size();
}

} // sched
} // oodles
