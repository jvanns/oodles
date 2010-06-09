// oodles
#include "utility/NodeIO.hpp"
#include "sched/Scheduler.hpp"
#include "utility/file-ops.hpp"
#include "utility/BreadCrumbTrail.hpp"

// STL
#include <vector>
#include <iostream>

// IO streams
using std::cout;
using std::cerr;
using std::endl;

// Containers
using std::string;
using std::vector;

// STL exception
using std::exception;

static
void
run_scheduler(oodles::sched::Scheduler &s, oodles::BreadCrumbTrail &t)
{
    const int n = 10;
    vector<oodles::sched::Crawler*> crawlers(n);

    for (int i = 0 ; i < n ; ++i) {
        crawlers[i] = new oodles::sched::Crawler(string(1, i + 'A'));
        s.register_crawler(*crawlers[i]);
    }

    const uint32_t assigned = s.run(&t);
    cout << "After 1 scheduling run " << assigned << " pages were assigned:\n";

    for (int i = 0 ; i < n ; ++i) {
        cout << '[' << crawlers[i]->id() << "]: "
             << crawlers[i]->unit_size() << "\n";
        delete crawlers[i];
    }
}

static
void
usage(const string &program)
{
    cerr << "Provide the location (file path) to a file containing URLs.\n";
    cerr << "usage: " << program << " <file> [--dot | --ascii]\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        return 1;
    }
    
    try {
        string s;
        size_t n = oodles::read_file_data(argv[1], s);

        if (n != s.size())
            return 1;

        oodles::BreadCrumbTrail trail;
        oodles::sched::Scheduler scheduler;
        string::size_type b = 0, e = s.find_first_of('\n', b), t = string::npos;

        while (e != t) {
            scheduler.schedule_from_seed(s.substr(b, e - b));

            b = e + 1;
            e = s.find_first_of('\n', b);
        }

        run_scheduler(scheduler, trail);

        if (!argv[2] || (argv[2] && strncmp(argv[2], "--ascii", 7) == 0)) {
            const oodles::io::ASCIIArt aa(scheduler.url_tree());
            cout << aa;
        } else if (argv[2] && (strncmp(argv[2], "--dot", 5) == 0)) {
            const oodles::io::DotMatrix dot(scheduler.url_tree());
            cout << dot;

            cout << "Replaying 1 scheduling run...\n";
            scheduler.replay_run(cout, trail);
        }
    } catch (const exception &e) {
        cerr << e.what();
        return 1;
    }

    return 0;
}
