#ifndef OODLES_TREEBASE_HPP
#define OODLES_TREEBASE_HPP

// oodles
#include "NodeBase.hpp"

namespace oodles {

/*
 * Bare-bones base class for any Tree type.
 * It must remain non-templated and defines
 * only necessary, common place methods and
 * attributes.
 */
class TreeBase
{
    public:
        TreeBase();
        virtual ~TreeBase();

        /*
         * Ensure this is a pure virtual (abstract) class
         */
        virtual const NodeBase& root() const = 0;
};

} // oodles

#endif

