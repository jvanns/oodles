// oodles scheduler
#include "sched/Context.hpp"

// oodles utility
#include "utility/file-ops.hpp"

// STL
#include <string>
#include <iostream>

// libc
#include <getopt.h>

// STL
using std::cout;
using std::cerr;
using std::endl;
using std::string;

// oodles utility
using oodles::read_file_data;
// oodles scheduler
using oodles::sched::Context;

static void print_usage(const char *program)
{
    cerr << program
         << ":\n"
         << "\n-h\t--help"
         << "\n-f\t--file <seed file>"
         << "\n-s\t--service <ip:port>\n";
}

int main(int argc, char *argv[])
{
    int ch = -1;
    string seed_file;
    string listen_on("127.0.0.1:8888");
    const char *short_options = "hf:s:";
    const struct option long_options[4] = {
        {"help", no_argument, NULL, short_options[0]},
        {"file", required_argument, NULL, short_options[1]},
        {"service", required_argument, NULL, short_options[3]},
        {NULL, 0, NULL, 0}
    };

    while ((ch = getopt_long(argc, argv,
                             short_options,
                             long_options, NULL)) != -1)
    {
        switch (ch) {
            case 'f':
                seed_file = optarg;
                break;
            case 's':
                listen_on = optarg;
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
        size_t n = read_file_data(seed_file, s);

        if (n != s.size())
            return 1;
        
        Context context;
        string::size_type b = 0, e = s.find_first_of('\n', b), t = string::npos;

        while (e != t) {
            context.seed_scheduler(s.substr(b, e - b));
            
            b = e + 1;
            e = s.find_first_of('\n', b);
        }

        context.start_server(listen_on);
        context.start_crawling(5);
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
        rc = 1;
    }

    return rc;
}
