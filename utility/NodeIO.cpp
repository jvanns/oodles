// oodles
#include "Tree.hpp"
#include "NodeIO.hpp"
#include "BreadCrumbTrail.hpp"

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
        n.print(s, *this);
        s << std::endl;
    }

    /*
     * Recursively print the child nodes (depth-first traverse)
     */
    for (size_t i = 0 ; i < n.size() ; ++i)
        print(s, n.child(i));

    return s;
}

DotMatrix::DotMatrix(const TreeBase &t) : PrinterBase(t), trail(NULL)
{
}

void
DotMatrix::set_trail(BreadCrumbTrail &t)
{
    trail = &t;
}

ostream&
DotMatrix::print(ostream &s, const NodeBase &n) const
{
    /*
     * Use the pointer addresses as the Node IDs
     */
    ptrdiff_t edge[2] = {reinterpret_cast<ptrdiff_t>(&n),
                         reinterpret_cast<ptrdiff_t>(n.parent)};

    /*
     * The root node is special, here we can set graph
     * options and begin the graph block DOT expects.
     */
    if (!n.parent) {
        s << "digraph oodles {\n\n"
          << edge[0] << " [label = \"ROOT\"];\n";
    }

    print_vertex(s, n);

    if (!trail) {
        print_edge(s, edge);

        /*
         * Recursively print the child nodes (depth-first traverse)
         */
        for (size_t i = 0 ; i < n.size() ; ++i)
            print(s, n.child(i));
    } else if (!trail->empty()) {
        const NodeBase *p = &n, /* p may get re-assigned here -------------*/
                       *q = const_cast<DotMatrix*>(this)->node_from_trail(p);

        if (q) {
            edge[0] = reinterpret_cast<ptrdiff_t>(&(*q));
            edge[1] = reinterpret_cast<ptrdiff_t>(&(*p));
            print_edge(s, edge);
            print(s, *q);
        }
    }

    /*
     * Terminate the graph block
     */
    if (!n.parent)
        s << "}\n";

    return s;
}

const NodeBase*
DotMatrix::node_from_trail(const NodeBase *&n)
{
    BreadCrumbTrail::Point i = trail->gather_crumb();

    if (!n->parent)
        i = trail->gather_crumb(); // Discard the root crumb

    /*
     * Determine the direction of travel; If the crumb path index is less
     * than that of the current node path index we're retreating back up
     * the way we came - back up the path to where we forked (in the road).
     */
    while (!trail->empty() && i.first < n->path_idx) {
        i = trail->gather_crumb(); // Gather until we reach the fork
        n = n->parent;
    }

    if (trail->empty())
        return NULL;

    /*
     * The crumb point cannot exceed the number of child nodes
     */
    assert(i.second < static_cast<BreadCrumbTrail::coord_t>(n->size()));

    return &n->child(i.second); // Next node located by the current crumb
}

void
DotMatrix::print_vertex(ostream &s, const NodeBase &n) const
{
    n.print(s, *this);
}

void
DotMatrix::print_edge(ostream &s, const ptrdiff_t e[2]) const
{
    if (e[1] == 0)
        return;

    /*
     * Link an edge between two node IDs.
     */
    s << e[1] << " -> " << e[0];
    
    if (trail)
        s << " [label=" << trail->gathered() << ']';

    s << ";\n";
}

ostream&
operator<< (ostream &s, const PrinterBase &p)
{
    return p(s);
}

} // io
} // oodles

