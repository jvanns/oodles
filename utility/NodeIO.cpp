// oodles
#include "Tree.hpp"
#include "NodeIO.hpp"

// STL
using std::ostream;

namespace oodles {
namespace io {

PrinterBase::PrinterBase(const TreeBase &t) : tree(t)
{
}

PrinterBase::~PrinterBase()
{
}

ostream&
PrinterBase::operator() (ostream &s) const
{
    return print(s, tree.root());
}

ASCIIArt::ASCIIArt(const TreeBase &t) : PrinterBase(t)
{
}

ostream&
ASCIIArt::print(ostream &s, const NodeBase &n) const
{
    static const std::string branch("+-- ");
    static const std::string::size_type len = branch.size() - 1;

    if (n.parent) {
        for (NodeBase::path_index_t i = 0 ; i < n.path_idx ; ++i) {
            s << '|';

            for (std::string::size_type j = 0 ; j < len ; ++j)
                s << ' ';
        }

        s << branch;
        n.print(s);
        s << std::endl;
    }

    /*
     * Recursively print the child nodes (depth-first traverse)
     */
    for (size_t i = 0 ; i < n.size() ; ++i)
        print(s, n.child(i));

    return s;
}

DotMatrix::DotMatrix(const TreeBase &t) : PrinterBase(t)
{
}

ostream&
DotMatrix::print(ostream &s, const NodeBase &n) const
{
    /*
     * Use the pointer addresses as the Node IDs
     */
    const ptrdiff_t nid = reinterpret_cast<ptrdiff_t>(&n),
                    pid = reinterpret_cast<ptrdiff_t>(n.parent);

    if (pid == 0) {
        /*
         * The root node is special, here we can set graph options
         */
        s << "digraph oodles {\n\n"
          << nid << " [label = \"ROOT\"];\n";
    } else {
        /*
         * Declare a node (using the pointer address as the ID)
         */
        s << nid << " [label=\"";
        n.print(s);
        s <<  '"';

        switch (n.visit_state) {
            case NodeBase::Red:
                s << ", color=red";
                break;
            case NodeBase::Green:
                s << ", color=green";
                break;
            case NodeBase::Amber:
                s << ", color=orange";
                break;
        }

        s << "];\n";

        /*
         * Link an edge between two node IDs.
         */
        s << pid << " -> " << nid << ";\n";
    }

    /*
     * Recursively print the child nodes (depth-first traverse)
     */
    for (size_t i = 0 ; i < n.size() ; ++i)
        print(s, n.child(i));

    /*
     * Terminate the graph block
     */
    if (pid == 0)
        s << "}\n";

    return s;
}

ostream&
operator<< (ostream &s, const PrinterBase &p)
{
    return p(s);
}

} // io
} // oodles

