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

class BreadCrumbTrail; // Forward declaration for Scheduler

namespace sched {

class Scheduler
{
    public:
        /* Member functions/methods */
        Scheduler();
        ~Scheduler();

        const TreeBase& url_tree() const { return tree; }

        bool register_crawler(Crawler &c); // Add a (new, unique) crawler

        void schedule_from_seed(const std::string &url);
        void schedule_from_crawl(const std::string &url);

        uint32_t run(BreadCrumbTrail *t = NULL); // Performs a scheduling run
        void replay_run(std::ostream &s, BreadCrumbTrail &t); // Replay a run
    private:
        /* Member variables/attributes */
        size_t leaves;
        BreadCrumbTrail *trail;
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
