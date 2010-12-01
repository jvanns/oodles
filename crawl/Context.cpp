// oodles
#include "Context.hpp"

// STL
using std::string;

namespace oodles {
namespace crawl {

Context::Context(const string &name, uint16_t cores) :
    dispatcher(cores),
    crawler(dispatcher, name, cores),
    client(dispatcher.io_service(), creator)
{
}

void
Context::stop_crawling()
{
    dispatcher.stop();
}

void
Context::start_crawling(const string &service)
{
    SchedulerCrawler &dialect = client.dialect();
    
    dialect.register_crawler(crawler.id(), crawler.cores());
    client.start(service);
    dispatcher.wait();
}

} // crawl
} // oodles
