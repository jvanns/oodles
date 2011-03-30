// oodles
#include "url/URL.hpp"
#include "common/Exceptions.hpp"
#include "utility/Dispatcher.hpp"

#include "net/oop/Session.hpp"
#include "net/oop/Protocol.hpp"
#include "net/oop/Messages.hpp"

#include "net/core/Client.hpp"
#include "net/core/Server.hpp"
#include "net/core/HandlerCreator.hpp"

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

class Session : public oodles::net::oop::Session
{
    public:
        void handle_message(oodles::net::oop::Message *m);
};

struct ClientContext : public oodles::net::CallerContext
{
    inline void start(oodles::net::SessionHandler &s)
    {
        start(static_cast<Session&>(s));
    }
    
    void start(Session &s)
    {
        using oodles::net::oop::RegisterCrawler;
        RegisterCrawler *m = new RegisterCrawler;

        m->name = "tarantula";
        m->cores = 8;

        s.push_message(m);
    }
};

struct ServerContext : public oodles::net::CallerContext
{
    void start(oodles::net::SessionHandler &s)
    {
        cout << "New client connection established.\n";
    }
};

void
Session::handle_message(oodles::net::oop::Message *m)
{
    using oodles::url::URL;
    using namespace oodles::net::oop;

    static const URL a("http://www.apple.com/uk"),
                     y("http://www.youtube.com/uk"),
                     f("http://www.facebook.co.uk");

    switch (m->id()) {
        case REGISTER_CRAWLER:
            {
            BeginCrawl *s = new BeginCrawl; // send
            RegisterCrawler &r = static_cast<RegisterCrawler&>(*m); // recv

            assert(r.name == "tarantula");
            assert(r.cores == 8);
            
            s->urls.push_back(&a);
            s->urls.push_back(&y);
            s->urls.push_back(&f);
            
            push_message(s);
            }
            break;
        case BEGIN_CRAWL:
            {
            EndCrawl *s = new EndCrawl; // send
            BeginCrawl &r = static_cast<BeginCrawl&>(*m); // recv
            list<const URL*>::const_iterator begin(r.urls.begin()),
                                                 end(r.urls.end());
            static const find<list<const URL*>::const_iterator, URL> finder;

            assert(r.urls.size() == 3);
            assert(finder(begin, end, a));
            assert(finder(begin, end, y));
            assert(finder(begin, end, f));
            
            s->scheduled_urls.push_back(make_pair(a.page_id(), true));
            s->scheduled_urls.push_back(make_pair(y.page_id(), true));
            s->scheduled_urls.push_back(make_pair(f.page_id(), true));

            push_message(s);
            }
            break;
        case END_CRAWL:
            {
            EndCrawl &r = static_cast<EndCrawl&>(*m); // recv

            assert(r.scheduled_urls.size() == 3);
            assert(r.new_urls.empty());
            }
            break;
        default:
            break;
    }
    
    delete m;
}

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

    argc -= optind;
    argv += optind;

    try {
        typedef oodles::net::oop::Protocol OOP;
        typedef oodles::net::Creator<OOP, Session> Creator;
        
        oodles::net::Server *server = NULL;
        oodles::net::Client *client = NULL;
        oodles::Dispatcher dispatcher;

        if (!client_only) {
            static ServerContext context;
            static const Creator creator(context);
            
            server = new oodles::net::Server(dispatcher, creator);
            server->start(listen_on);
        }

        if (!server_only) {
            static ClientContext context;
            static const Creator creator(context);
            
            client = new oodles::net::Client(dispatcher, creator);
            client->start(connect_to);
        }

        dispatcher.wait();
        delete client;
        delete server;
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
