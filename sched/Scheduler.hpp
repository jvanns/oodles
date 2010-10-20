#ifndef OODLES_SCHED_SCHEDULER_HPP
#define OODLES_SCHED_SCHEDULER_HPP

// oodles
#include "Node.hpp"
#include "Crawler.hpp"
#include "utility/Tree.hpp"
#include "utility/Linker.hpp"

// STL
#include <queue>
#include <string>
#include <tr1/unordered_map>

namespace oodles {

class BreadCrumbTrail; // Forward declaration for Scheduler

namespace sched {

class Context; // Forward declaration for Scheduler

class Scheduler : public Linker
{
    public:
        /* Member functions/methods */
        Scheduler(Context *c = NULL);
        ~Scheduler();

        Context* context() { return ctxt; }
        const TreeBase& url_tree() const { return tree; }
        void register_crawler(Crawler &c) { crawlers.push(&c); }

        void schedule_from_seed(const std::string &url);
        void schedule_from_crawl(const std::string &url);

        uint32_t run(BreadCrumbTrail *t = NULL); // Performs a scheduling run
    private:
        /* Member variables/attributes */
        Context *ctxt;
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
