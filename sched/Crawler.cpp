// oodles
#include "Crawler.hpp"

namespace  oodles {
namespace sched {

Crawler::Crawler() : endpoint(NULL)
{
}

bool
RankCrawler::operator() (const Crawler *lhs, const Crawler *rhs) const
{
    /*
     * First, be sure to check that the Crawler has a valid Endpoint!
     */
    if (!lhs->endpoint)
        return false;

    /*
     * Always prefer Crawlers with less work at the front of the queue
     */
    return lhs->work_unit.size() < rhs->work_unit.size();
}

} // sched
} // oodles
