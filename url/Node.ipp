#ifndef OODLES_URL_NODE_IPP // Implementation
#define OODLES_URL_NODE_IPP

// STL
#include <iostream>

namespace oodles {
namespace url {

template<class T>
Node<T>::Node() : path_id(0), node_id(0), parent(NULL) {}

template<class T>
Node<T>::Node(const T &v, path_index_t i) :
    value(v),
    path_id(i),
    node_id(0),
    parent(NULL)
{
}

template<class T>
Node<T>::~Node()
{
    for (iterator i = children.begin() ; i != children.end() ; ++i)
        delete *i;
}

/*
 * TODO: There is potential for optimisation here. First, profile
 * common and extreme use of the URL tree and discern whether or
 * not it is necessary to swap-out the use of vector<> and has_child
 * for a better data structure such as a BST or even a RBT (set<>).
 */
template<class T>
inline
bool
Node<T>::has_child(const T &v, Node<T> *&c) const
{
    if (children.empty())
        return false;
    
    for (iterator i = children.begin() ; i != children.end() ; ++i) {
        if (v == (*i)->value) {
            c = *i;
            return true;
        }
    }

    return false;
}

template<class T>
static
inline
std::ostream& operator<< (std::ostream &stream, const Node<T> &node)
{
    stream << node.value << ": "
           << "PID(" << node.path_id << "), "
           << "NID(" << node.node_id << ")";

    if (node.parent)
        stream << ", PPID(" << node.parent->path_id << ")";

    return stream << ", Children: " << node.children.size();
}

} // url
} // oodles

#endif

