#ifndef OODLES_NODE_IPP // Implementation
#define OODLES_NODE_IPP

// STL
#include <iostream>

namespace oodles {

template<class T>
inline
Node<T>*
Node<T>::create_child(const T &v, path_index_t i)
{
    Node<T> *n = new Node<T>();

    n->value = v;
    n->path_idx = i;
    n->child_idx = children.size();

    n->parent = this;
    children.push_back(n);

    return n;
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
Node<T>::has_child(const T &v, Node *&c) const
{
    if (children.empty())
        return false;

    return search(0, children.size() - 1, v, c);
}

template<class T>
Node<T>::Node() : path_idx(0), child_idx(0), parent(NULL) {}

template<class T>
Node<T>::~Node()
{
    for (iterator i = children.begin() ; i != children.end() ; ++i)
        delete *i;
}

/*
 * Recursive divide and conquer algorithm for searching a
 * simple, unordered array of items (child nodes).
 */
template<class T>
inline
bool
Node<T>::search(const size_t l, const size_t r, const T &v, Node *&c) const
{
    if (l == r) {
        if (v == children[l]->value) {
            c = children[l];
            return true;
        }

        return false;
    }

    const size_t m = (l + r) / 2;

    if (search(l, m, v, c))
        return true;

    return search(m + 1, r, v, c);
}

template<class T>
static
inline
std::ostream& operator<< (std::ostream &stream, const Node<T> &node)
{
    stream << node.value << ": "
           << "PID(" << node.path_idx << "), "
           << "NID(" << node.child_idx << ")";

    if (node.parent)
        stream << ", PPID(" << node.parent->path_idx << ")";

    return stream << ", Children: " << node.children.size();
}

} // oodles

#endif

