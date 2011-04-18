#ifndef OODLES_NET_HTTP_MESSAGE_HPP
#define OODLES_NET_HTTP_MESSAGE_HPP

// oodles
#include "common/Exceptions.hpp"

// STL
#include <vector>
#include <string>

namespace oodles {
namespace net {
namespace http {

/*
 * Generic HTTP Message - should handle both Request and Response
 * types. Written using RFC2616 as a reference and guide. Note
 * that when writing the body, if any, it is written to a file
 * descriptor and the offset saved within Message as an X-header.
 *
 * The fd may point to any external resource such as a regular file,
 * a socket or pipe, a shared memory location etc.
 */
class Message
{
    public:
        /* Member functions/methods */
        Message(int fd = -1, size_t size = 0);

        /* Client-side interface */
        uint16_t response_code() const;
        const std::string& response_phrase() const;
        void request(const std::string &method, const std::string &URI); // Send
        
        /* Server-side interface */
        const std::string& request_URI() const;
        const std::string& request_method() const; 
        void respond(uint16_t code, const std::string &phrase); // Send

        /* Generic Request & Response message interface */
        size_t size() const;
        const std::string& find_header(const std::string &key) const;
        bool add_header(const std::string &key, const std::string &value);
        
        /* Protocol handler interface */
        bool complete() const;
        size_t to_buffer(char *buffer, size_t max);
        size_t from_buffer(const char *buffer, size_t max); 
    private: 
        /* Forward declarations */
        struct Header;
        
        /* Member functions/methods */
        size_t pending() const;
        bool headers_sent() const { return headers.empty(); }
        bool headers_received() const { return body_offset > 0; }

        /* Message construction from network buffer (recv) */
        size_t read_body(const char *buffer, size_t max);
        void read_response_line(const char *buffer, size_t max);
        size_t read_headers(const char *buffer,size_t max) throw (HeaderError);

        /* Message deconstruction to network buffer (send) */
        size_t write_body(char *buffer, size_t max);
        size_t write_headers(char *buffer,size_t max);
        size_t write_request_line(char *buffer, size_t max); 
        
        /* Internal structures */
        enum { Inbound = 1, Outbound = 2 };
        
        class Trigram
        {
            public:
                size_t size() const { return tokens[0].size() + 
                                             tokens[1].size() +
                                             tokens[2].size(); }
                std::string& operator[] (size_t i) { return tokens[i]; }
                const std::string& operator[] (size_t i) const { return tokens[i]; }
            private:
                std::string tokens[3];
        };

        struct Header
        {
            Header() {}
            Header(const std::string &key, const std::string &value) :
                key(key),
                value(value)
            {}
            
            bool operator< (const Header &other) const;
            bool operator== (const Header &other) const;
            bool operator< (const std::string &key) const;
            bool operator== (const std::string &key) const;
            
            std::string key, value;
        };

        /* Member variables/attributes */
        int fd;
        int mode;
        float version;
        Trigram start_line;
        std::vector<Header> headers;
        size_t buffered, body_offset, body_size;

        static const char CR, LF;
        static const std::string protocol;
};

} // http
} // net
} // oodles

#endif

