#ifndef OODLES_NODEBASE_HPP
#define OODLES_NODEBASE_HPP

// STL
#include <iostream>

// libc
#include <stdint.h> // For int16_t

namespace oodles {
namespace io {
    
class PrinterBase; // Forward declration for Node

} // io

/*
 * Bare-bones base class for any Node type.
 * It must remain non-templated and defines
 * only necessary, common place methods and
 * attributes.
 */
class NodeBase
{
    public:
        /* Dependent typedefs */
        typedef int16_t path_index_t; // Depth
        typedef int32_t child_index_t; // Breadth

        /* Member functions/methods */
        NodeBase();
        virtual ~NodeBase();

        /*
         * Ensure this is a pure virtual (abstract) class and
         * indicate that these methods *must* be provided by
         * derived Node types.
         */
        virtual NodeBase& child(size_t index) = 0;
        virtual const NodeBase& child(size_t index) const = 0;

        virtual size_t size() const = 0;
        virtual void print(std::ostream &s, const io::PrinterBase &p) const = 0;

        bool leaf() const { return parent && size() == 0; }
        
        /*
         * Cast operators for casting up the class hierarchy to
         * our concrete, derived subclass - generally a Node<T>.
         */
        template<class Node>
        operator Node&() { return static_cast<Node&>(*this); }

        template<class Node>
        operator const Node&() const { return static_cast<const Node&>(*this); }

        /*
         * Visitation state (using traffic light colours ;)
         * Use when traversing the tree to mark clean/dirty nodes/branches
         */
        enum {
            Red = 0, // This node and all children have been visited
            Green = 1, // This node not visited nor any of it's children
            Amber = 2 // This node visited but not yet all it's children
        };

        /* Member variables/attributes */
        int visit_state; // Visitation state
        NodeBase *parent; // Pointer to parent node
        path_index_t path_idx; // Index of this node within it's path
        child_index_t child_idx; // Index of this node amoung it's siblings
};

} // oodles

#endif
