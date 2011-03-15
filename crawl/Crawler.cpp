// oodles
#include "Crawler.hpp"

// STL
using std::string;

namespace oodles {
namespace crawl {

Crawler::Crawler(const string &name, uint16_t cores, Dispatcher &d) :
    cpus(cores), /* We use cpus/cores interchangeably */
    name(name),
    dispatcher(d)
{
}

void
Crawler::fetch(const url::URL &url)
{
    urls[url.domain_id()].push_back(url);
}

} // crawl
} // oodles

