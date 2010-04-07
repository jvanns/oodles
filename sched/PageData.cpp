// oodles
#include "Crawler.hpp"
#include "PageData.hpp"

// STL
using std::string;

namespace oodles {
namespace sched {

PageData::PageData(const string &url) :
    crawler(NULL),
    url(url),
    referrer(NULL),
    epoch(0),
    last_crawl(0),
    links(0),
    crawl_count(0),
    paralleled(0)
{
}

void
PageData::assign_crawler(Crawler *c)
{
    crawler = c;
    crawler->add_url(&url);
}

} // oodles
} // sched
