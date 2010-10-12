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
using oodles::net::Protocol;
using oodles::net::oop::dialect::SchedulerCrawler;

typedef oodles::net::oop::Protocol OOP;

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
    string connect_to("127.0.0.1:8888");
    const char *short_options = "hc:s:";
    const struct option long_options[4] = {
        {"help", no_argument, NULL, short_options[0]},
        {"cores", required_argument, NULL, short_options[1]},
        {"scheduler", required_argument, NULL, short_options[3]},
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
        
        const Protocol<OOP, SchedulerCrawler> creator;
        Client client(proactor.io_service(), creator);
        SchedulerCrawler &dialect = client.dialect();
        
        client.start(connect_to);
        dialect.register_crawler("tarantula", cores);

        proactor.wait();
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        rc = 1;
    }

    return rc;
}
