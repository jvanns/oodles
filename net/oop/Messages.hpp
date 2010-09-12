#ifndef OODLES_NET_OOP_MESSAGES_HPP
#define OODLES_NET_OOP_MESSAGES_HPP

// oodles
#include "url/URL.hpp"
#include "Serialiser.hpp"

// STL
#include <map>
#include <list>
#include <string>

// libc
#include <stdint.h> // For uint16_t

namespace oodles {
namespace net {
namespace oop {
namespace msg {

/*
 * This namespace is private. Do not use it. It holds the message
 * implementations as simple structs. However, these are typedef'd
 * later outside of this namespace - use these. These can then be
 * serialised and cast correctly.
 */

struct RegisterCrawler_
{
    /* Member functions/methods */
    static id_t id() { return REGISTER_CRAWLER; }
    void serialize(Reconstructor &archive, unsigned int /* version */);
    void serialize(Deconstructor &archive, unsigned int /* version */) const;
    
    /* Member variables/attributes */
    uint16_t cores;
    std::string name;
};

struct BeginCrawl_
{
    /* Dependent typedefs */
    //................Page ID...........Page URL
    typedef std::pair<url::URL::hash_t, std::string> URL;
    
    /* Member functions/methods */
    static id_t id() { return BEGIN_CRAWL; }
    void serialize(Reconstructor &archive, unsigned int /* version */);
    void serialize(Deconstructor &archive, unsigned int /* version */) const;
    
    /* Member variables/attributes */
    //.......Domain ID.........URLs
    std::map<url::URL::hash_t, std::list<URL> > urls;
};

struct EndCrawl_
{
    /* Dependent typedefs */
    //................Page URL.....Crawled?
    typedef std::pair<std::string, bool> URL;
    
    /* Member functions/methods */
    static id_t id() { return END_CRAWL; }
    void serialize(Reconstructor &archive, unsigned int /* version */);
    void serialize(Deconstructor &archive, unsigned int /* version */) const;
    
    /* Member variables/attributes */
    std::list<URL> new_urls;
    //.......Page ID...........Success?
    std::map<url::URL::hash_t, bool> scheduled_urls;
};

} // msg

typedef Serialiser<msg::RegisterCrawler_> RegisterCrawler;
typedef Serialiser<msg::BeginCrawl_> BeginCrawl;
typedef Serialiser<msg::EndCrawl_> EndCrawl;

} // oop
} // net
} // oodles

#endif
