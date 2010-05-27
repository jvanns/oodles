// oodles
#include "sched/Scheduler.hpp"
#include "utility/file-ops.hpp"

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

void
run_scheduler(oodles::sched::Scheduler &s)
{
    const int n = 10;
    vector<oodles::sched::Crawler*> crawlers(n);

    for (int i = 0 ; i < n ; ++i) {
        crawlers[i] = new oodles::sched::Crawler(string(1, i + 'A'));
        s.register_crawler(*crawlers[i]);
    }

    const uint32_t assigned = s.run();
    cout << "After 1 scheduling run " << assigned << " pages were assigned:\n";

    for (int i = 0 ; i < n ; ++i) {
        cout << '[' << crawlers[i]->id() << "]: "
             << crawlers[i]->unit_size() << "\n";
        delete crawlers[i];
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) { 
        cerr << "Provide the location (file path) to a file containing URLs.\n";
        return 1; 
    }
    
    try {
        string s;
        size_t n = oodles::read_file_data(argv[1], s);

        if (n != s.size())
            return 1;

        oodles::sched::Scheduler scheduler;
        string::size_type b = 0, e = s.find_first_of('\n', b), t = string::npos;

        while (e != t) {
            scheduler.schedule_from_seed(s.substr(b, e - b));

            b = e + 1;
            e = s.find_first_of('\n', b);
        }

        run_scheduler(scheduler);
    } catch (const exception &e) {
        cerr << e.what();
        return 1;
    }

    return 0;
}
