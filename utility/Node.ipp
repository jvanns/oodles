#ifndef OODLES_NODE_IPP // Implementation
#define OODLES_NODE_IPP

#include <algorithm>

namespace oodles {

/*
 * Public methods
 */
template<class T>
bool
Node<T>::leaf() const
{
    return parent && children.empty(); // A leaf node should never have children
}

template<class T>
void
Node<T>::print(std::ostream &stream) const
{
    static const std::string branch("+-- ");
    static const std::string::size_type len = branch.size() - 1;

    if (parent) {
        for (path_index_t i = 0 ; i < path_idx ; ++i) {
            stream << '|';

            for (std::string::size_type j = 0 ; j < len ; ++j)
                stream << ' ';
        }

        stream << branch << value << std::endl;
    }

    for (iterator i = children.begin() ; i != children.end() ; ++i)
        stream << *(*i);
}

template<class T>
Node<T>*
Node<T>::create_child(const T &v, path_index_t i)
{
    KeyCmp cmp;
    typename std::vector<Node<T>*>::iterator x =
        std::lower_bound(children.begin(), children.end(), v, cmp);

    /*
     * Check if lower_bound() returned an existing
     * node with a key equal to this value...
     */
    if (x != children.end() && (*x)->value == v)
        return *x;

    Node *n = new_node(v);

    n->path_idx = i;
    n->child_idx = children.size();

    n->parent = this;
    children.insert(x, n);

    return n;
}

/*
 * Protected methods
 */
template<class T>
Node<T>::Node(const T &v) :
    value(v),
    path_idx(0),
    child_idx(0),
    parent(NULL),
    visit_state(Black)
{
}

template<class T>
Node<T>::~Node()
{
    for (iterator i = children.begin() ; i != children.end() ; ++i)
        delete *i;
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
Node<T>::Node() : path_idx(0), child_idx(0), parent(NULL), visit_state(Black)
{
}

} // oodles

#endif

