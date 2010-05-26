// oodles
#include "NodeBase.hpp"

namespace oodles {

NodeBase::NodeBase() :
    visit_state(Green),
    parent(NULL),
    path_idx(0),
    child_idx(0)
{
}

NodeBase::~NodeBase()
{
    const_iterator i = children.begin(), j = children.end();

    while (i != j) {
        delete *i;
        ++i;
    }
}

} // oodles
