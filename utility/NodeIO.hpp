#ifndef OODLES_NODE_IO_HPP
#define OODLES_NODE_IO_HPP

// oodles
#include "TreeBase.hpp"

// STL
#include <iostream>

namespace oodles {

class BreadCrumbTrail; // Forward declaration for DotMatrix

namespace io {

/*
 * Base class for all stream-based output.
 */
struct PrinterBase
{
    public:
        /* Member functions/methods */
        std::ostream& operator() (std::ostream &s) const;
    protected:
        /* Member functions/methods */
        PrinterBase(const TreeBase &t);
        virtual ~PrinterBase();

        virtual std::ostream& print(std::ostream &s,
                                    const NodeBase &n) const = 0;

        /* Member variables/attributes */
        const TreeBase &tree;
};

/*
 * Straight-forward ASCII tree printer
 */
class ASCIIArt : public PrinterBase
{
    public:
        /* Member functions/methods */
        ASCIIArt(const TreeBase &t);
    private:
        /* Member functions/methods */
        std::ostream& print(std::ostream &s, const NodeBase &n) const;
};

/*
 * Ha ha ha, see what I did here?
 * Printer for Nodes that outputs
 * in the DOT language format ;)
 */
class DotMatrix : public PrinterBase
{
    public:
        /* Member functions/methods */
        DotMatrix(const TreeBase &t);

        void set_trail(BreadCrumbTrail &t);
    private:
        /* Member functions/methods */
        std::ostream& print(std::ostream &s, const NodeBase &n) const;

        const NodeBase* node_from_trail(const NodeBase *&n);
        void print_vertex(std::ostream &s, const NodeBase &n) const;
        void print_edge(std::ostream &s, const ptrdiff_t e[2]) const;

        /* Member variables/attributes */
        BreadCrumbTrail *trail;
};

std::ostream&
operator<< (std::ostream &s, const PrinterBase &d);

} // io
} // oodles

#endif
