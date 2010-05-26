#ifndef OODLES_NODEBASE_HPP
#define OODLES_NODEBASE_HPP

// STL
#include <vector>
#include <iostream>

namespace oodles {

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
        typedef uint16_t path_index_t; // Depth
        typedef uint32_t child_index_t; // Breadth
        typedef std::vector<NodeBase*>::iterator iterator;
        typedef std::vector<NodeBase*>::const_iterator const_iterator;

        /* Member functions/methods */
        NodeBase();
        virtual ~NodeBase();

        /*
         * Ensure this is a pure virtual (abstract) class
         */
        virtual void print(std::ostream &stream) const = 0;

        size_t size() const { return children.size(); }
        bool leaf() const { return parent && size() == 0; }
        
        NodeBase& child(size_t index) { return *children[index]; }
        const NodeBase& child(size_t index) const { return *children[index]; }
        
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
        std::vector<NodeBase*> children; // This node's children
};

} // oodles

#endif
