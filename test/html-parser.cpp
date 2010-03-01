// oodles
#include "html/Parser.hpp"
#include "utility/file-ops.hpp"

// Boost.foreach
#include <boost/foreach.hpp>

// STL
#include <iostream>

// IO streams
using std::cout;
using std::cerr;
using std::endl;

// Containers
using std::string;

static
void
tab(uint16_t indent)
{
    for (uint16_t i = 0 ; i < indent ; ++i)
        cout << ' ';
}

namespace oodles {

struct Element_printer
{
    Element_printer(uint16_t i = 0) : indent(i) {}
    void operator() (const html::Element &e) const;

    uint16_t indent;
};

struct Node_printer : boost::static_visitor<>
{
    Node_printer(uint16_t i = 0) : indent(i) {}
    void operator() (const html::Text &t) const;
    void operator() (const html::Element &e) const;

    uint16_t indent;
};

void
Element_printer::operator() (const html::Element &e) const
{
    tab(indent); // Print indentation for improved readability of output
    cout << '<' << e.tag.name; // Print the opening tag name

    /* And any attributes associated with the tag */
    if (!e.tag.attributes.empty()) {
        html::Tag::Attributes::const_iterator i = e.tag.attributes.begin(),
                                              j = e.tag.attributes.end();
        while (i != j) {
            cout << ' ' << i->first << "=\"" << i->second << '"';
            ++i;
        }
    }

    cout << ">\n"; // Terminate opening tag

    BOOST_FOREACH(const html::Node &child, e.children) {
        boost::apply_visitor(Node_printer(indent), child);
    }
    
    tab(indent); // Print indentation for improved readability of output
    cout << "</" << e.tag.name << ">\n";
}

void
Node_printer::operator() (const html::Text &t) const
{
    tab(indent + 3); // Print indentation for improved readability
    cout << t << endl;
}

void
Node_printer::operator() (const html::Element &e) const
{
    Element_printer(indent + 3)(e);
}

} // oodles

int main(int argc, char *argv[]) {
    if (!argv[1]) {
        cerr << "Provide the location (file path) to an HTML document.\n";
        return 1;
    }

    string s;
    const int n = read_file_data(argv[1], s);

    if (n == -1) {
        cerr << "Failed to open " << argv[1] << " for reading.\n";
        return 1;
    }

    if (n != static_cast<int>(s.size())) {
        cout << "No. bytes read into buffer not equal to size of input.\n";
        return 1;
    }

    cout << "Read " << n << " bytes from " << argv[1] << ".\n";

    string::const_iterator b = s.begin(), e = s.end(); // Input iterators
    oodles::html::Parser<string::const_iterator> p; // Our parser

    // Parse the document!
    if (p.parse(b, e)) {
        cout << "Document parsed successfully and in full.\n";
        oodles::Element_printer()(p.document());
        return 0;
    }

    cout << "Failed to parse document.\n";

    return 1;
}
