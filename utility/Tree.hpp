#ifndef OODLES_TREE_HPP // Interface
#define OODLES_TREE_HPP

// oodles
#include "Node.hpp"

// STL
#include <vector>

namespace oodles {

template<class Type>
class Tree
{
    public:
        /* Dependent typedefs */
        typedef typename Node<Type>::path_index_t path_index_t;

        /* Member functions/methods */
        Tree(Node<Type> *n = NULL);
        ~Tree();

        void print(std::ostream &stream) const;

        template<class Iterator>
        Node<Type>* insert(Iterator b, // Beginning
                           Iterator e, // End
                           Node<Type> *p = NULL); // Parent node

        const Node<Type>& root() const { return *seed; }

        void depth_first_traverse(Node<Type> &n) const;
        void breadth_first_traverse(Node<Type> &n) const;
    private:
        /* Member functions/methods */
        void visit(Node<Type> &n) const;
        Node<Type>* insert(const Type &v, path_index_t i, Node<Type> &p);

        /* Member variables/attributes */
        Node<Type>* seed; // Roots grow from seed - and root() is already used!
};

template<class Type>
static
inline
std::ostream&
operator<< (std::ostream &stream, const Tree<Type> &tree)
{
    tree.print(stream);
    return stream;
}

} // oodles

#include "Tree.ipp" // Implementation

#endif

