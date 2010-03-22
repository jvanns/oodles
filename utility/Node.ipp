#ifndef OODLES_NODE_IPP // Implementation
#define OODLES_NODE_IPP

namespace oodles {

/*
 * Public methods
 */
template<class T>
inline
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
        return false; // Really no point in continuing ;)

    bool r = false;
    size_t i = 0, s = children.size();

    switch (s) {
        case 1:
            r = children[i]->value == v;
            break;
        default:
            r = search(0, s - 1, v, i);
            break;
    }

    if (r)
        c = children[i];

    return r;
}

template<class T>
inline
Node<T>*
Node<T>::create_child(const T &v, path_index_t i)
{
    Node *n = new_node(v);

    n->path_idx = i;
    n->child_idx = children.size();

    n->parent = this;
    children.push_back(n);

    if (n->child_idx) { // More than one child already
        child_index_t r = n->child_idx, l = r - 1;

        while (children[l]->value > children[r]->value) {
            std::swap(children[l], children[r]); // Maintain an ordered list

            if (l == 0)
                break;

            --r;
            --l;
        }
    }

    return n;
}

/*
 * Protected methods
 */
template<class T>
Node<T>::~Node()
{
    for (iterator i = children.begin() ; i != children.end() ; ++i)
        delete *i;
}

template<class T>
Node<T>::Node(const T &v) : value(v), path_idx(0), child_idx(0), parent(NULL)
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
Node<T>::Node() : path_idx(0), child_idx(0), parent(NULL)
{
}

/*
 * Recursive divide and conquer algorithm for searching a
 * simple, unordered array of items (child nodes).
 */
template<class T>
inline
bool
Node<T>::search(const size_t l, const size_t r, const T &v, size_t &n) const
{
    bool c = false; // c = condition

    if (l == r) {
        if (v == children[l]->value) {
            c = true;
            n = l;
        }

        return c;
    }

    const size_t m = (l + r) / 2; // Split the range in to two halves

    if (v <= children[m]->value)
        c = search(l, m, v, n); // Search the 'left' half
    else
        c = search(m + 1, r, v, n); // Search the 'right' half

    return c;
}

} // oodles

#endif

