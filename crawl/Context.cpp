// oodles
#include "Context.hpp"
#include "utility/Linker.hpp"

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
    const Link link(&client, &crawler);
    
    dialect.register_crawler(crawler);
    client.start(service);
    dispatcher.wait();
}

} // crawl
} // oodles
