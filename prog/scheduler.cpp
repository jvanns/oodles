// oodles scheduler
#include "sched/Context.hpp"

// oodles utility
#include "utility/file-ops.hpp"

// STL
#include <string>
#include <fstream>
#include <iostream>

// libc
#include <getopt.h>
#include <signal.h> // For sigaddset() etc.

// STL
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ostream;
using std::ofstream;
using std::exception;

// oodles utility
using oodles::read_file_data;
// oodles scheduler
using oodles::sched::Context;

static Context *g_context = NULL;

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

    if (stop && g_context)
        g_context->stop_crawling();
}

static void set_signal_handler(Context &c)
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

    g_context = &c;
}

static void print_usage(const char *program)
{
    cerr << program
         << ":\n"
         << "\n-h\t--help"
         << "\n-s\t--service <ip:port>"
         << "\n-f\t--seed-file <seed input file>"
         << "\n-d\t--dot-file <dot output file>\n";
}

int main(int argc, char *argv[])
{
    int ch = -1, interval = 5;
    string seed_file, dot_file;
    string listen_on("127.0.0.1:8888");
    const char *short_options = "hs:f:d:i:";
    const struct option long_options[6] = {
        {"help", no_argument, NULL, short_options[0]},
        {"service", required_argument, NULL, short_options[1]},
        {"seed-file", required_argument, NULL, short_options[3]},
        {"dot-file", required_argument, NULL, short_options[5]},
        {"interval", required_argument, NULL, short_options[7]},
        {NULL, 0, NULL, 0}
    };

    while ((ch = getopt_long(argc, argv,
                             short_options,
                             long_options, NULL)) != -1)
    {
        switch (ch) {
            case 'd':
                dot_file = optarg;
                break;
            case 'f':
                seed_file = optarg;
                break;
            case 's':
                listen_on = optarg;
                break;
            case 'i':
                interval = atoi(optarg);
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (seed_file.empty()) {
        print_usage(argv[0]);
        return 1;
    }

    int rc = 0;
    
    try {
        string s;
        ostream *dot_stream = NULL;
        size_t n = read_file_data(seed_file, s);

        if (n != s.size())
            return 1;

        if (!dot_file.empty()) {
            try {
                dot_stream = new ofstream(dot_file.c_str());
            } catch (const exception &e) {
                cerr << "Failed to open '" << dot_file << "' for writing.\n";
                return 1;
            }
        }
        
        Context context;
        string::size_type b = 0, e = s.find_first_of('\n', b), t = string::npos;

        /*
         * We must allow the signal handler to stop the
         * proactor service and join all the threads.
         */
        set_signal_handler(context);

        /* Allow Crawlers to sit connected */
        context.start_server(listen_on);

        while (e != t) {
            context.seed_scheduler(s.substr(b, e - b));
            
            b = e + 1;
            e = s.find_first_of('\n', b);
        }

        s.clear(); // We no longer need this data
        context.start_crawling(dot_stream, interval); // interval between runs

        delete dot_stream;
    } catch (const exception &e) {
        cerr << e.what() << endl;
        rc = 1;
    }

    return rc;
}
