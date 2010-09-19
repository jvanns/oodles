#ifndef OODLES_NODE_IPP // Implementation
#define OODLES_NODE_IPP

// STL
#include <typeinfo>
#include <algorithm>

namespace oodles {

/*
 * Public methods
 */
template<class T>
Node<T>*
Node<T>::create_child(const T &v, path_index_t i)
{
    Node *n = new_node(v);
    const std::pair<size_t, bool> x(children.insert(n));

    if (!x.second) {
        /*
         * Node with value v already existed - return it.
         */
        Node &r = child(x.first);
        delete n;
        n = &r;
    } else {
        /*
         * Node with value v is new - initialise it.
         */
        n->path_idx = i;
        n->parent = this;
        n->child_idx = x.first;
    }

    return n;
}
 
template<class T>
void
Node<T>::print(std::ostream &s, const io::PrinterBase &p) const
{
    if (typeid(p) == typeid(io::DotMatrix)) {
        /*
         * Declare a node (using the pointer address as the ID)
         */
        const ptrdiff_t nid = reinterpret_cast<ptrdiff_t>(this);
        s << nid << " [label=\"" << value << '"';

        switch (visit_state) {
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
    } else {
        s << value;
    }
}

/*
 * Protected methods
 */
template<class T>
Node<T>::Node(const T &v) : NodeBase(), value(v), children(true)
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
Node<T>::Node() : children(true)
{
}

} // oodles

#endif

