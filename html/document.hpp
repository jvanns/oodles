#ifndef OODLES_HTML_DOCUMENT_H
#define OODLES_HTML_DOCUMENT_H

// Boost.recursive_variant
#include <boost/variant/recursive_variant.hpp>

// STL
#include <map>
#include <string>
#include <vector>

namespace oodles {
namespace html {

struct Element; // Forward declaration of Element for Node
typedef std::string Text; // Early typedef of Text for Node

/*
 * A Node can either be another HTML Element or it could
 * be a block of Text (i.e. content or Javascript etc.).
 */
typedef boost::variant<boost::recursive_wrapper<Element>, Text> Node;

/*
 * And there may be many of these (child) nodes per Element
 */
typedef std::vector<Node> Nodes;

struct Tag
{
    typedef std::string Name;
    typedef std::map<std::string, std::string> Attributes;

    Name name; // Tag name
    Attributes attributes; // Tag attributes
};

struct Element
{
    Tag tag; // Tag
    Nodes children; // Child nodes
    bool terminated; // Some tags aren't :|

    Element() : terminated(false) {}
};

} // html
} // oodles

#endif
