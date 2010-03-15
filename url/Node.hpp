#ifndef OODLES_URL_NODE_HPP // Interface
#define OODLES_URL_NODE_HPP

// STL
#include <vector>

#include <stdint.h> // For uint16_t

namespace oodles {
namespace url {

template<class T>
struct Node
{
    typedef uint16_t path_index_t;
    typedef uint16_t node_index_t;
    typedef uint32_t tree_index_t;

    const T value;
    path_index_t path_id; // Index of this node within it's path
    node_index_t node_id; // Index of this node amoung it's siblings
    tree_index_t tree_id; // Index of this node according to the whole tree

    Node *parent;
    std::vector<Node<T>*> children;

    Node();
    Node(const T &v, path_index_t i);
    bool has_child(const T &v, Node<T> *&c) const;
};

} // url
} // oodles

#include "Node.ipp" // Implementation

#endif

