// oodles
#include "Crawler.hpp"
#include "PageData.hpp"

// STL
using std::string;

namespace oodles {
namespace sched {

PageData::PageData(const string &url) :
    url(url),
    crawler(NULL),
    referrer(NULL),
    last_crawl(0),
    epoch(time(NULL)),
    links(0),
    crawl_count(0)
{
}

void
PageData::assign_crawler(Crawler *c)
{
    assert(!crawler);
    
    c->add_url(url);
    crawler = c;
}

void
PageData::unassign_crawler()
{
    assert(crawler);
    
    crawler->remove_url(url);
    crawler = NULL;
}

} // oodles
} // sched
