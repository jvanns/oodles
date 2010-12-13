// oodles
#include "url/URL.hpp"
#include "common/Exceptions.hpp"

// oodles core networking
#include "net/core/Client.hpp"
#include "net/core/Server.hpp"
#include "net/core/ProtocolCreator.hpp"

// oodles protocol (OOP)
#include "net/oop/Factory.hpp"
#include "net/oop/Messages.hpp"
#include "net/oop/Protocol.hpp"

// STL
#include <string>
#include <iostream>

// libc
#include <getopt.h>

// STL
using std::list;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::make_pair;

namespace {

/*
 * Find a value but dereference the pointer as well as the iterator
 */
template<typename Iterator, typename Type> struct find
{
    find() {}

    bool operator() (Iterator i, Iterator j, const Type &value) const
    {
        while (i != j) {
            if (*(*i) == value)
                return true;
            
            ++i;
        }

        return false;
    }
};

class TestOOPDialect : public oodles::net::ProtocolDialect
{
    public:
        /* Member functions/methods */
        typedef oodles::url::URL URL;
        TestOOPDialect() : initiator(false) {}
        
        void register_crawler()
        {
            using oodles::net::oop::RegisterCrawler;
            
            RegisterCrawler *m = new RegisterCrawler;
            m->name = "tarantula";
            m->cores = 8;

            initiator = true;
            
            send(m); // Non-blocking
        }
        
        void translate()
        {
            oodles::net::oop::Message *m = protocol().pop_message();

            while (m) {
                continue_dialog(m);
                m = protocol().pop_message();
            }
        }
    private:
        /* Internal Data Structures */
        struct Context
        {
            oodles::net::oop::id_t inbound, outbound;
            Context() : inbound(oodles::net::oop::INVALID_ID),
                        outbound(oodles::net::oop::INVALID_ID) {}
        };
        
        /* Member variables/attributes */
        bool initiator;
        Context context;
        
        /* Member functions/methods */
        void continue_dialog(oodles::net::oop::Message *m)
        {
            using namespace oodles::net::oop;
            cout << "Received OOP message ID #" << m->id() << endl;
            
            /*
             * Check the dialog context (based on received messages)
             */
            switch (m->id()) {
                case REGISTER_CRAWLER: // Scheduler receives from Crawler
                    assert(!initiator);
                    assert(context.inbound == INVALID_ID);
                    assert(context.outbound == INVALID_ID);
                    
                    continue_dialog(static_cast<RegisterCrawler&>(*m)); 
                    break;
                case BEGIN_CRAWL: // Crawler receives from Scheduler
                    assert(initiator);
                    assert(context.inbound == INVALID_ID // New crawler
                           || context.inbound == BEGIN_CRAWL); // Last crawl
                    assert(context.outbound == REGISTER_CRAWLER // New crawler
                           || context.outbound == END_CRAWL); // Last crawl
                    
                    continue_dialog(static_cast<BeginCrawl&>(*m)); 
                    break;
                case END_CRAWL: // Scheduler receives from Crawler
                    assert(!initiator);
                    assert(context.inbound == REGISTER_CRAWLER // 1st Crawl
                           || context.inbound == END_CRAWL); // Subsequent crawl
                    assert(context.outbound == BEGIN_CRAWL); // Only ever begin
                    
                    continue_dialog(static_cast<EndCrawl&>(*m)); 
                    break;
            }

            context.inbound = m->id();
            delete m;
        }
        
        void continue_dialog(const oodles::net::oop::RegisterCrawler &r)
        {
            assert(r.name == "tarantula");
            assert(r.cores == 8);
            new_crawl();
        }
        
        void continue_dialog(const oodles::net::oop::BeginCrawl &b)
        {
            assert(b.urls.size() == 3);
            
            static const oodles::url::URL a("http://www.apple.com/uk"),
                                          y("http://www.youtube.com/uk"),
                                          f("http://www.facebook.co.uk");
            static const find<list<URL*>::const_iterator, URL> finder;
            list<URL*>::const_iterator begin(b.urls.begin()), end(b.urls.end());

            assert(finder(begin, end, a));
            assert(finder(begin, end, y));
            assert(finder(begin, end, f));
            
            using oodles::net::oop::EndCrawl;

            EndCrawl *m = new EndCrawl;
            m->scheduled_urls.push_back(make_pair(a.page_id(), true));
            m->scheduled_urls.push_back(make_pair(y.page_id(), true));
            m->scheduled_urls.push_back(make_pair(f.page_id(), true));

            send(m); // Non-blocking
        }
        
        void continue_dialog(const oodles::net::oop::EndCrawl &e)
        {
            assert(e.scheduled_urls.size() == 3);
            assert(e.new_urls.empty());
            new_crawl();
        }

        void new_crawl()
        {
            using oodles::net::oop::BeginCrawl;

            BeginCrawl *m = new BeginCrawl;
            static oodles::url::URL a("http://www.apple.com/uk"),
                                    y("http://www.youtube.com/uk"),
                                    f("http://www.facebook.co.uk");

            m->urls.push_back(&a);
            m->urls.push_back(&y);
            m->urls.push_back(&f);
            
            send(m); // Non-blocking
        }
        
        void send(oodles::net::oop::Message *m)
        {
            context.outbound = m->id();
            protocol().push_message(m);
            cout << "Sending OOP message ID #" << context.outbound << endl;
        }

        oodles::net::oop::Protocol& protocol() const
        {
            assert(handler != NULL);
            return *handler;
        }
};

} // anonymous

static void print_usage(const char *program)
{
    cerr << program << " [-h|-c <host:port>|-s <ip:port>]\n\n";

    cerr << "Provide -c (or --client) and a host:port pair to run a client.\n";
    cerr << "Provide -s (or --server) and an ip:port pair to run a server.\n";
}

int main(int argc, char *argv[])
{
    int ch = -1;
    bool client_only = false,
         server_only = false;
    string listen_on("127.0.0.1:8888"),
           connect_to("localhost:8888");
    const char *short_options = "hc:s:";
    const struct option long_options[4] = {
        {"help", no_argument, NULL, short_options[0]},
        {"client", required_argument, NULL, short_options[1]},
        {"server", required_argument, NULL, short_options[3]},
        {NULL, 0, NULL, 0}
    };

    while ((ch = getopt_long(argc, argv,
                             short_options,
                             long_options, NULL)) != -1)
    {
        switch (ch) {
            case 'c':
                connect_to = optarg;
                client_only = true;
                break;
            case 's':
                listen_on = optarg;
                server_only = true;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    int rc = 0;
    const bool send = (!client_only && !server_only) || client_only;
    
    oodles::net::Server *server = NULL;
    oodles::net::Client *client = NULL;
    typedef oodles::net::oop::Protocol OOP;
    const oodles::net::Protocol<OOP, TestOOPDialect> creator;

    try {
        boost::asio::io_service service;

        if (!client_only)
            server = new oodles::net::Server(service, creator);

        if (!server_only)
            client = new oodles::net::Client(service, creator);

        if (send) {
            TestOOPDialect &d = client->dialect();
            d.register_crawler();
        }

        if (server)
            server->start(listen_on);

        if (client)
            client->start(connect_to);
        
        service.run();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        rc = 1;
    }

    delete client;
    delete server;

    return rc;
}
