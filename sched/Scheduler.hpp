#ifndef OODLES_SCHED_SCHEDULER_HPP
#define OODLES_SCHED_SCHEDULER_HPP

// oodles
#include "Node.hpp"
#include "Crawler.hpp"
#include "utility/Tree.hpp"

// STL
#include <queue>
#include <string>
#include <tr1/unordered_map>

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
        void register_crawler(Crawler &c) { crawlers.push(&c); }

        void schedule_from_seed(const std::string &url);
        void schedule_from_crawl(const std::string &url);

        /*
         * NOTE: Look at boost.signals for publish/subscribe events system.
         * Using signals + slots we can quickly execute callbacks when
         * events external to the Scheduler happen. For example, the receipt
         * of a crawl confirmation message will contain the page ID and with
         * this we can weigh_tree_branch() again from after retrieving the
         * node by the ID via our hashtable, page_table.
         */
        uint32_t run(BreadCrumbTrail *t = NULL); // Performs a scheduling run
    private:
        /* Member variables/attributes */
        size_t leaves;
        BreadCrumbTrail *trail;
        Tree<Node::value_type> tree;
        std::priority_queue<Crawler*,
                            std::deque<Crawler*>,
                            RankCrawler> crawlers;

        /*
         * We already have a hash function in URL that identifies the
         * page, path and domain. TR1 requires the hash functor to
         * take a key type and return a hash of type size_t. We
         * simply return the hash (because that's all we'll be passing
         * around outside of the Scheduler) or id() cast to size_t.
         */
        struct hash_id
        {
            size_t operator() (url::URL::hash_t h) const { return h; }
        };
        std::tr1::unordered_map<url::URL::hash_t, Node*, hash_id> page_table;

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
