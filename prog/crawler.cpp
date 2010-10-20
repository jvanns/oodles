// oodles
#include "utility/Proactor.hpp"
#include "net/core/Client.hpp"
#include "net/oop/SchedulerCrawler.hpp"

// STL
#include <string>
#include <iostream>

// libc
#include <stdlib.h> // For atoi()
#include <getopt.h> // For getopt()

// STL
using std::cout;
using std::cerr;
using std::endl;
using std::string;

// oodles
using oodles::Proactor;
using oodles::net::Client;
using oodles::net::hostname;
using oodles::net::Protocol;
using oodles::net::oop::dialect::SchedulerCrawler;

typedef oodles::net::oop::Protocol OOP;

static Proactor *g_proactor = NULL;

static void signal_handler(int signal)
{
    bool stop = false;

    switch (signal) {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
            stop = true;
            break;
        default:
            break;
    }

    if (stop && g_proactor)
        g_proactor->stop();
}

static void set_signal_handler(Proactor &p)
{
    sigset_t blocked;
    struct sigaction action;

    sigemptyset(&blocked);
    memset(&action, '\0', sizeof(struct sigaction));

    // Block these signals when actually in the handler
    sigaddset(&blocked, SIGINT);
    sigaddset(&blocked, SIGTERM);
    sigaddset(&blocked, SIGQUIT);

    action.sa_mask = blocked;
    action.sa_handler = &signal_handler;
    action.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    // We have special ways of handling these signals
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);

    g_proactor = &p;
}

static void print_usage(const char *program)
{
    cerr << program
         << ":\n"
         << "\n-h\t--help"
         << "\n-c\t--cores <#cores>"
         << "\n-s\t--scheduler <hostname:port>\n";
}

int main(int argc, char *argv[])
{
    int ch = -1;
    uint16_t cores = 1;
    string name(hostname());
    string connect_to("127.0.0.1:8888");
    const char *short_options = "hn:c:s:";
    const struct option long_options[5] = {
        {"help", no_argument, NULL, short_options[0]},
        {"name", required_argument, NULL, short_options[1]},
        {"cores", required_argument, NULL, short_options[3]},
        {"scheduler", required_argument, NULL, short_options[5]},
        {NULL, 0, NULL, 0}
    };

    while ((ch = getopt_long(argc, argv,
                             short_options,
                             long_options, NULL)) != -1)
    {
        switch (ch) {
            case 'c':
                cores = atoi(optarg);
                break;
            case 'n':
                name = optarg;
                break;
            case 's':
                connect_to = optarg;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    int rc = 0;
    
    try {
        Proactor proactor(cores);

        /*
         * We must allow the signal handler to stop the
         * proactor service and join all the threads.
         */
        set_signal_handler(proactor);
        
        const Protocol<OOP, SchedulerCrawler> creator;
        Client client(proactor.io_service(), creator);
        SchedulerCrawler &dialect = client.dialect();

        client.start(connect_to);
        dialect.register_crawler(name, cores);

        proactor.wait();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        rc = 1;
    }

    return rc;
}
