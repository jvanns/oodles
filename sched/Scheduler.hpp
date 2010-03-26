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
        Scheduler();
        ~Scheduler();

        void schedule_from_seed(const std::string &url);
        void schedule_from_crawl(const std::string &url);
    private:
        /* Member variables/attributes */
        Tree<Node::value_type> tree;

        /* Member functions/methods */
        void schedule(const std::string &url, bool from_seed);
};

} // sched
} // oodles

#endif