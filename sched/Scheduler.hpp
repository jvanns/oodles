#ifndef OODLES_SCHED_SCHEDULER_HPP
#define OODLES_SCHED_SCHEDULER_HPP

// oodles
#include "Node.hpp"
#include "Crawler.hpp"
#include "utility/Tree.hpp"

// STL
#include <queue>
#include <string>

namespace oodles {
namespace sched {

class Scheduler
{
    public:
        /* Member functions/methods */
        Scheduler();
        ~Scheduler();

        uint32_t run(); // Performs scheduling run
        bool register_crawler(Crawler &c); // Add a crawler

        void schedule_from_seed(const std::string &url);
        void schedule_from_crawl(const std::string &url);
    private:
        /* Member variables/attributes */
        size_t leaves;
        Tree<Node::value_type> tree;
        std::priority_queue<Crawler*,
                            std::deque<Crawler*>,
                            RankCrawler> crawlers;

        /* Member functions/methods */
        Node* traverse_branch(Node &n) const;
        void weigh_tree_branch(Node &n) const;
        Node* select_best_node(Node &parent) const;
        
        void fill_crawler(Crawler &c);
        void schedule(const std::string &url, bool from_seed);
};

} // sched
} // oodles

#endif
