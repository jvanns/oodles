// oodles
#include "url/URL.hpp"
#include "utility/Tree.hpp"
#include "utility/NodeIO.hpp"
#include "utility/file-ops.hpp"

// STL
#include <iostream>

// libc
#include <string.h> // For strncmp()

// IO streams
using std::cout;
using std::cerr;
using std::endl;

// Containers
using std::string;

// STL exception
using std::exception;

static oodles::Tree<std::string> tree;

static
bool
build_url(const string &line) // line from the input file
{
    try {
        oodles::url::URL url(line);
        tree.insert(url.begin_tree(), url.end_tree());
    } catch (const exception &e) {
        cerr << e.what();
        return false;
    }
    
    return true;
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

        string::size_type b = 0, e = s.find_first_of('\n', b), t = string::npos;

        while (e != t) {
            build_url(s.substr(b, e - b));

            b = e + 1;
            e = s.find_first_of('\n', b);
        }

        if (!argv[2] || (argv[2] && strncmp(argv[2], "--ascii", 7) == 0)) {
            const oodles::io::ASCIIArt aa(tree);
            cout << aa;
        } else if (argv[2] && (strncmp(argv[2], "--dot", 5) == 0)) {
            const oodles::io::DotMatrix dot(tree);
            cout << dot;
        }
    } catch (const exception &e) {
        cerr << e.what();
        return 1;
    }

    return 0;
}
