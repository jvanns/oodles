#ifndef OODLES_NODE_IPP // Implementation
#define OODLES_NODE_IPP

#include <algorithm>

namespace oodles {

/*
 * Public methods
 */

template<class T>
void
Node<T>::print(std::ostream &stream) const
{
    stream << value;
}

template<class T>
Node<T>*
Node<T>::create_child(const T &v, path_index_t i)
{
    KeyCmp cmp;
    iterator x = std::lower_bound(children.begin(), children.end(), v, cmp);

    /*
     * Check if lower_bound() returned an existing
     * node with a key equal to this value...
     */
    if (x != children.end()) {
        Node<T> &n = *(*x); // All items in 'children' are NodeBase pointers
        
        if (n.value == v)
            return &n;
    }

    Node *n = new_node(v);

    n->path_idx = i;
    n->parent = this;

    /*
     * FIXME: Ack! We must re-index all child indicies after x
     * This is awful and will impact on performance terribly :(
     */
    for (iterator q = children.insert(x, n) ; q != children.end() ; ++q)
        (*q)->child_idx = q - children.begin();

    return n;
}

/*
 * Protected methods
 */
template<class T>
Node<T>::Node(const T &v) : NodeBase(), value(v)
{
}

template<class T>
Node<T>::~Node()
{
}

template<class T>
void
Node<T>::visit()
{
}

/*
 * Virtual method that should be overwritten when derived from to be sure
 * the instance of Node is of the correct derived type offering extended
 * use.
 */
template<class T>
Node<T>*
Node<T>::new_node(const T &v) const
{
    return new Node(v);
}

/*
 * Private methods
 */
template<class T>
Node<T>::Node()
{
}

} // oodles

#endif

