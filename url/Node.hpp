#ifndef OODLES_URL_NODE_HPP // Interface
#define OODLES_URL_NODE_HPP

// STL
#include <vector>

#include <stdint.h> // For uint16_t

namespace oodles {
namespace url {

template<class T> class Tree; // Forward declaration for friend below

template<class T>
class Node
{
    public:
        /* Dependent typedefs */
        typedef uint16_t path_index_t;
        typedef uint16_t child_index_t;

        /* Member functions/methods */
        bool has_child(const T &v, Node *&c) const;
        Node* create_child(const T &v, path_index_t i);

        /* Member variables/attributes */
        T value; // Value at this node
        path_index_t path_idx; // Index of this node within it's path
        child_index_t child_idx; // Index of this node amoung it's siblings

        Node *parent; // Pointer to parent node
        std::vector<Node<T>*> children; // This nodes children
    private:
        /* Dependent typedefs */
        typedef typename std::vector<Node<T>*>::const_iterator iterator;

        /* Member functions/methods */
        Node();
        ~Node();

        Node(const Node &n);
        Node& operator=(const Node &n);

        friend class Tree<T>; // Tree wants Node() only.
};

} // url
} // oodles

#include "Node.ipp" // Implementation

#endif

