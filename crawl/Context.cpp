// oodles
#include "Context.hpp"
#include "utility/Linker.hpp"

// STL
using std::string;

namespace {

struct BeginDialog : public oodles::net::OnConnect
{
    oodles::net::OnConnect* create() const { return new BeginDialog(*this); }

    void operator() (oodles::net::ProtocolHandler &p)
    {
        oodles::net::oop::dialect::SchedulerCrawler &d = *p.get_dialect();
        d.register_crawler();
    }

    static BeginDialog& instance()
    {
        static BeginDialog x;
        return x;
    }
};
 
} // anonymous

namespace oodles {
namespace crawl {

Context::Context(const string &name, uint16_t cores) :
    dispatcher(cores),
    crawler(dispatcher, name, cores),
    creator(&BeginDialog::instance()),
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
    const Link link(&client, &crawler);
    
    client.start(service);
    dispatcher.wait();
}

} // crawl
} // oodles
