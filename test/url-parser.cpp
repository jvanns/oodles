// oodles
#include "url/URL.hpp"
#include "utility/file-ops.hpp"

// STL
#include <iostream>

// IO streams
using std::cout;
using std::cerr;
using std::endl;

// Containers
using std::string;

// STL exception
using std::exception;

static
bool
build_url(const string &line) // line from the input file
{
    try {
        cout << "Input URL: " << line << endl;
        oodles::URL url(line);
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

        string::size_type b = 0, e = s.find_first_of('\n', e), t = string::npos;

        while (e != t) {
            build_url(s.substr(b, ++e));

            b = e;
            e = s.find_first_of('\n', b);
        }
    } catch (const exception &e) {
        cerr << e.what();
        return 1;
    }

    return 0;
}
