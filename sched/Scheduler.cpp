// oodles
#include "Scheduler.hpp"

// STL
using std::string;

namespace oodles {
namespace sched {

void
Scheduler::schedule(const string &url)
{
    url::URL *u = new url::URL(url);
    Node *leaf = static_cast<Node*> (tree.insert(u->begin_tree(),
                                                 u->end_tree()));

    if (!leaf->url) // Unique insert
        leaf->url = u; // Ownership of 'u' is implicitly transferred here
    else
        ; // URL was a duplicate (hit already logged by Tree::insert)
}

}
}

