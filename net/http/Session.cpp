// oodles
#include "Session.hpp"
#include "Message.hpp"
#include "Protocol.hpp"
#include "ResponseCodes.hpp"

namespace oodles {
namespace net {
namespace http {

void
Session::handle_messages()
{
    Protocol &p = *static_cast<Protocol*>(get_endpoint()->get_protocol());
    
    for (Message *m = p.pop_message() ; m ; m = p.pop_message())
        handle_message(m);
}

void
Session::send_request(const std::string &resource, int response_fd)
{
    Protocol &p = *static_cast<Protocol*>(get_endpoint()->get_protocol());
    Message *m = new Message();
    
    m->add_header("host", get_endpoint()->local_hostname());
    m->request("GET", resource);
    
    p.push_message(m, response_fd);
}

void
Session::send_response(uint16_t code, int data_fd, size_t data_size)
{
    Protocol &p = *static_cast<Protocol*>(get_endpoint()->get_protocol());
    static const ResponseCodes &messages = ResponseCodes::instance();
    Message *m = new Message(data_fd, data_size);
    
    m->respond(code, messages[code]);
    p.push_message(m);
}


} // http
} // net
} // oodles

