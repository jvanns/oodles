// oodles
#include "url/URL.hpp"
#include "utility/file-ops.hpp"

// STL
#include <iostream>

// IO streams
using std::cout;
using std::cerr;
using std::endl;
using std::ostream_iterator;

// Algorithms
using std::copy;

// Containers
using std::string;

// STL exception
using std::exception;

static
bool
build_url(const string &line) // line from the input file
{
    try {
        oodles::URL url(line);
        cout << "Tokenised URL: " << url << endl;

        cout << "Iterating through URL: ";
        copy(url.begin(), url.end(),
             ostream_iterator<oodles::URL::value_type>(cout));
        cout << endl;
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
