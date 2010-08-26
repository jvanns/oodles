// oodles
#include "Message.hpp"

// libc
#include <string.h> // For memcpy()

namespace oodles {
namespace net {
namespace oop {

const uint8_t Message::Header::header_size = sizeof(uint32_t) + sizeof(id_t);

Message::Message(id_t id, uint32_t body_size)
{
    header.message_id = id;
    header.body_size = body_size;
}

Message::~Message()
{
}

id_t
Message::id() const
{
    return INVALID_ID;
}

void
Message::header2buffer(char *buffer, Header &h)
{
    static const uint8_t size_width = sizeof(uint32_t), id_width = sizeof(id_t);
    const uint32_t message_size = htonl(h.header_size + h.body_size);
    const id_t message_id = htons(h.message_id);

    memcpy(buffer, &message_size, size_width);
    memcpy(buffer + size_width, &message_id, id_width);
    h.processed = true;
}

Message::Header
Message::buffer2header(const char *buffer)
{
    static const uint8_t size_width = sizeof(uint32_t), id_width = sizeof(id_t);
    uint32_t message_size = 0;
    id_t message_id = 0;
    Header h;
    
    memcpy(&message_size, buffer, size_width);
    memcpy(&message_id, buffer + size_width, id_width);

    h.body_size = ntohl(message_size) - h.header_size;
    h.message_id = ntohs(message_id);
    h.processed = true;

    return h;
}

} // oop
} // net
} // oodles
