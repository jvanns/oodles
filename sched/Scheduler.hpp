#ifndef OODLES_SCHED_SCHEDULER_HPP
#define OODLES_SCHED_SCHEDULER_HPP

// oodles
#include "Node.hpp"
#include "Crawler.hpp"
#include "utility/Tree.hpp"
#include "utility/BreadCrumbTrail.hpp"

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

        const TreeBase& url_tree() const { return tree; }

        uint32_t run(); // Performs a scheduling run
        void replay_run(std::ostream &s); // With the BCT of the last run()
        bool register_crawler(Crawler &c); // Add a (new, unique) crawler

        void schedule_from_seed(const std::string &url);
        void schedule_from_crawl(const std::string &url);
    private:
        /* Member variables/attributes */
        size_t leaves;
        BreadCrumbTrail trail;
        Tree<Node::value_type> tree;
        std::priority_queue<Crawler*,
                            std::deque<Crawler*>,
                            RankCrawler> crawlers;

        /* Member functions/methods */
        Node* traverse_branch(Node &n);
        void weigh_tree_branch(Node &n) const;
        Node* select_best_child(Node &parent) const;
        
        Crawler::unit_t fill_crawler(Crawler &c, Node *&n);
        void schedule(const std::string &url, bool from_seed);
};

} // sched
} // oodles

#endif
