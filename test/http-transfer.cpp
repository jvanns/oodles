// oodles
#include "url/URL.hpp"

#include "utility/Linker.hpp"
#include "utility/Dispatcher.hpp"

#include "common/Exceptions.hpp"

#include "net/http/Message.hpp"
#include "net/http/Session.hpp"
#include "net/http/Protocol.hpp"

#include "net/core/Client.hpp"
#include "net/core/Server.hpp"
#include "net/core/HandlerCreator.hpp"

// STL
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

// libc
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>

// STL
using std::map;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::vector;
using std::string;
using std::ostringstream;

namespace {

/*
 * Constants
 */
static const string tmp("/tmp/HTTP/"); // Web-root for server

template<typename T>
inline
string
to_string(const T &t)
{
    ostringstream ss;
    ss << t;
    return ss.str();
}

class Session : public oodles::net::http::Session
{
    public:
        void handle_message(oodles::net::http::Message *m);
        void request_resources(const vector<string> &resources);
};

struct ClientContext : public oodles::net::CallerContext, public oodles::Linker
{
    map<string, vector<string> > requests;
    
    void start(oodles::net::SessionHandler &s);
    void start(Session &s);
};

struct ServerContext : public oodles::net::CallerContext, public oodles::Linker
{
    map<string, int> resources;
    
    void start(oodles::net::SessionHandler &s);
    void start(Session &s);
    
    int open_file(const string &resource);
    size_t size_from_fd(int fd);
};

// Session
void
Session::handle_message(oodles::net::http::Message *m)
{
    oodles::Linker *complement = coupling->complement_of(*this);
    ServerContext *s = dynamic_cast<ServerContext*>(complement);

    if (s) {
        int fd = s->open_file(m->request_URI());
        size_t size = fd != -1 ? s->size_from_fd(fd) : 0;
        
        cout << "[SRV] Request: " << m->request_method()
             << " '" << m->request_URI() << "'\n";

        send_response(fd != -1 ? 200 : 404, fd, size);
    } else {
        ClientContext *c = dynamic_cast<ClientContext*>(complement);
        cout << "[CLI] Response code: " << m->response_code() << endl;
    }
    
    delete m;
}

void
Session::request_resources(const vector<string> &resources)
{
    vector<string>::const_iterator i(resources.begin()), j(resources.end());

    for ( ; i != j ; ++i) {
        cout << "CLI: Requesting '" << *i << "'...\n";
        send_request(*i, fileno(stdout));
    }
}

// Client
void
ClientContext::start(oodles::net::SessionHandler &s)
{
    cout << "CLI: Connection established with server." << endl;
    start(static_cast<Session&>(s));
}

void
ClientContext::start(Session &s)
{
    const oodles::Link l(this, &s);
    const string &server = s.get_endpoint()->remote_hostname();
    const string port(to_string<uint16_t>(s.get_endpoint()->remote_port()));

    s.request_resources(requests[server + ":" + port]);
}

// Server
void
ServerContext::start(oodles::net::SessionHandler &s)
{
    cout << "SRV: Connection established with client." << endl;
    start(static_cast<Session&>(s));
}

void
ServerContext::start(Session &s)
{
    const oodles::Link l(this, &s);
}

int
ServerContext::open_file(const string &resource)
{
    typedef pair<string, int> Item;
    typedef map <string, int>::iterator Iterator;
    
    const Item item(resource, -1);
    const pair<Iterator, bool> entry(resources.insert(item));
    
    if (entry.second) {
        const string filename(tmp + resource);
        entry.first->second = open(filename.c_str(), O_RDONLY, 0);
    }

    return entry.first->second;
}

size_t
ServerContext::size_from_fd(int fd)
{
    assert(fd >= 0);
    struct stat b;
    fstat(fd, &b);
    
    return b.st_size;
}

} // anonymous

static void print_usage(const char *program)
{
    cerr << program << " [-h|-c|-s <ip:port>] <URL URL>\n\n";
    cerr << "Provide -s (or --server) and an ip:port pair to run a server.\n";
}

int main(int argc, char *argv[])
{
    int ch = -1;
    bool run_client = true,
         run_server = true;
    string listen_on("127.0.0.1:8888");
    const char *short_options = "hcs:";
    const struct option long_options[4] = {
        {"help", no_argument, NULL, short_options[0]},
        {"client", no_argument, NULL, short_options[1]},
        {"server", required_argument, NULL, short_options[3]},
        {NULL, 0, NULL, 0}
    };

    while ((ch = getopt_long(argc, argv,
                             short_options,
                             long_options, NULL)) != -1)
    {
        switch (ch) {
            case 'c':
                run_server = false;
                break;
            case 's':
                listen_on = optarg;
                run_client = false;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    argc -= optind;
    argv += optind;

    if (run_client && argc == 0) {
        cerr << "Supply at least one URL to request.\n";
        return 1;
    }

    try {
        typedef oodles::net::http::Protocol HTTP;
        typedef oodles::net::Creator<HTTP, Session> Creator;

        oodles::Dispatcher dispatcher;
        oodles::net::Server *server = NULL;
        oodles::net::Client *client = NULL;

        if (run_server) {
            static ServerContext context;
            static const Creator creator(context);
            
            cout << "NOTE: All documents are served from " << tmp << ".\n";
            server = new oodles::net::Server(dispatcher, creator);
            server->start(listen_on);
        }

        if (run_client) {
            static ClientContext context;
            static const vector<string> empty;
            static const Creator creator(context);
            map<string, vector<string> > &requests = context.requests;
        
            typedef pair<string, vector<string> > Item;
            typedef map <string, vector<string> >::iterator Iterator;

            client = new oodles::net::Client(dispatcher, creator);
            for (int i = 0 ; i < argc ; ++i) {
                string port("80");
                const oodles::url::URL url(argv[i]);

                if (!url.port().empty())
                    port = url.port();

                const Item item(url.host() + ":" + port, empty);
                const pair<Iterator, bool> entry(requests.insert(item));

                if (entry.second)
                    client->start(entry.first->first);
                
                entry.first->second.push_back(url.resource());
            }
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
