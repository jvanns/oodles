// oodles
#include "url/URL.hpp"
#include "utility/file-ops.hpp"

// STL
#include <iostream>
#include <algorithm>

// IO streams
using std::cout;
using std::cerr;
using std::endl;
using std::ostream_iterator;

// Algorithms
using std::find;
using std::count;

// Containers
using std::string;

// STL exception
using std::exception;

static
bool
build_url(const string &line) // line from the input file
{
    try {
        oodles::url::URL url(line);
        cout << "Tokenised URL: " << url << endl;
        cout << "Domain, Path and Page document IDs: "
             << url.domain_id() << ", "
             << url.path_id() << ", "
             << url.page_id() << endl;

        cout << "Testing iterator access...";
        if (find(url.begin(), url.end(), "http") != url.end())
            cout << "(http scheme), ";

        if (find(url.begin(), url.end(), "index.html") != url.end())
            cout << "(.html index page), ";

        if (count(url.begin(), url.end(), "www") > 0)
            cout << "(www host).";
        cout << endl;

        cout << "Testing bidirectional iteratation...";
        for (oodles::url::URL::iterator i = url.end() ; i != url.begin() ; )
            cout << *(--i) << ",";
        cout << endl << endl;
    } catch (const exception &e) {
        cerr << e.what();
        return false;
    }
    
    return true;
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

        string::size_type b = 0, e = s.find_first_of('\n', b), t = string::npos;

        while (e != t) {
            build_url(s.substr(b, e - b));

            b = e + 1;
            e = s.find_first_of('\n', b);
        }
    } catch (const exception &e) {
        cerr << e.what();
        return 1;
    }

    return 0;
}
