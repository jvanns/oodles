#ifndef OODLES_NODE_IPP // Implementation
#define OODLES_NODE_IPP

#ifdef HAVE_OPENMP
// OpenMP
#include <omp.h>
#endif

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

/*
 * TODO: There is potential for optimisation here. First, profile
 * common and extreme use of the URL tree and discern whether or
 * not it is necessary to swap-out the use of vector<> and has_child
 * for a better data structure such as a BST or even a RBT (set<>).
 */
template<class T>
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
            r = find(0, s - 1, v, i);
            break;
    }

    if (r)
        c = children[i];

    return r;
}

template<class T>
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

template<class T>
bool
Node<T>::find(const size_t l, const size_t r, const T &v, size_t &n) const
#ifdef HAVE_OPENMP
/*
 * Use a parallel loop to search the array between n threads
 * as scheduled by OpenMP and the following directives.
 */
{
    static size_t threads = 0;
   
    if (!threads) {
        #pragma omp parallel
        {
            #pragma omp single
            {
                threads = omp_get_num_threads();
                #pragma omp flush(threads)
            }
        }
    }
    
    bool found = false;
    size_t id = 0, i = 0, j = 0, p = 0;
    const size_t chunk = 512, min = threads * chunk;
    const size_t batch = (r >= min ? r / threads : r);
    #pragma omp parallel if(r >= min) shared(found) private(id, i, j, p)
    {
        id = omp_get_thread_num(); // Thread ID
        i = (batch * id) + id; // Range begin
        j = batch * (id + 1); // Range end
        p = 0; // Position of item, if found.

        if (bsearch(i, j, v, p, found)) {
            #pragma omp critical
            {
                found = true; // Should cancel other threads
                n = p;
            }

            #pragma omp flush(found)
        }
    }

    return found;
}
#else
{
    size_t p = 0;
    static const bool cancel = false;
    const bool found = bsearch(l, r, v, p, cancel);

    if (found)
        n = p;
    
    return found;
}
#endif
/*
 * Binary search implemented as a recursive divide and
 * conquer algorithm for searching a simple, ordered
 * array of items (child nodes).
 */
template<class T>
bool
Node<T>::bsearch(size_t l,
                 size_t r,
                 const T &v,
                 size_t &n,
                 const bool &cancel) const
{
#ifdef HAVE_OPENMP
    if (cancel)
        return false; // If we've been cancelled, we haven't found 'v'!
#endif

    if (l == r) {
        const bool found = v == children[l]->value;

        if (found)
            n = l;

        return found;
    }

    const size_t m = (l + r) / 2; // Split the range in to two halves

    if (v <= children[m]->value)
        return bsearch(l, m, v, n, cancel);  // Search the 'left' half

    return bsearch(m + 1, r, v, n, cancel); // Search the 'right' half
}

} // oodles

#endif

