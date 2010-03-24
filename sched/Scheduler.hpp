#ifndef OODLES_SCHED_SCHEDULER_HPP
#define OODLES_SCHED_SCHEDULER_HPP

// oodles
#include "Node.hpp"
#include "utility/Tree.hpp"

// STL
#include <string>

namespace oodles {
namespace sched {

class Scheduler
{
    public:
        /* Member functions/methods */
        void schedule(const std::string &url);
    private:
        /* Member variables/attributes */
        Tree<Node::value_type> tree;
};

} // sched
} // oodles

#endif
