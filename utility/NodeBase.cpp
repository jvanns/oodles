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
}

} // oodles
