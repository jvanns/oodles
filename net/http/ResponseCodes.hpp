#ifndef OODLES_NET_HTTP_RESPONSECODES_HPP
#define OODLES_NET_HTTP_RESPONSECODES_HPP

// oodles
#include "utility/BitSet.hpp"

// STL
#include <vector>
#include <string>

namespace oodles {
namespace net {
namespace http {

class ResponseCodes
{
    public:
        /* Member functions/methods */
        static ResponseCodes& instance();
        const std::string& operator[] (size_t code) const;
    private:
        /* Member functions/methods */
        ResponseCodes();
        ~ResponseCodes();
        void add_message(uint16_t code, const std::string &message);

        /* Member variables/attributes */
        BitSet index;
        std::vector<std::string> messages;
};

} // http
} // net
} // oodles

#endif
