#include "Engine/Networking/Message.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

namespace Net {

Message::Message(const MessageID& messageType)
    : FileUtils::BinaryStream()
    , sender(nullptr)
    , payload{}
    , payload_write_bytes(0)
    , payload_read_bytes(0)
    , message_type_index(messageType)
{
    /* DO NOTHING */
}

Message::~Message() {
    sender = nullptr;
}

unsigned int Message::read_bytes(void* out_buffer, const unsigned int count) {
    unsigned int new_count = count;
    if(payload_read_bytes + new_count > payload_write_bytes) {
        ERROR_AND_DIE("Message::read_bytes: Payload size too big.");
    }
    std::memcpy(out_buffer, payload.data() + payload_read_bytes, new_count);
    payload_read_bytes += new_count;
    return new_count;
}

unsigned int Message::write_bytes(const void* buffer, const unsigned int size) const {
    //if(this->HostEndianOrder() == FileUtils::BIG_ENDIAN) {
    //    CopyReversed(reinterpret_cast<unsigned char*>(buffer), buffer, size);
    //}
    unsigned int new_size = size;
    if(payload_write_bytes + new_size > payload.size()) {
        ERROR_AND_DIE("Message::write_bytes: Payload size too big.");
    }
    std::memcpy((void*)(payload.data() + payload_write_bytes), buffer, new_size);
    payload_write_bytes += new_size;
    return new_size;
}

const Net::MessageID& Message::GetMessageType() const {
    return message_type_index;
}

void Message::writeString(const std::string& str) {
    uint16_t new_size = (uint16_t)str.size();
    if(new_size > 0xfffe) {
        new_size = 0xfffe;
    }

    *((uint16_t*)(payload.data() + payload_write_bytes)) = new_size;
    std::memcpy((void*)(payload.data() + 2 + payload_write_bytes), str.data(), new_size);
    payload_write_bytes += new_size + 2;
}

void Message::writeString(const char* str) {
    //Special nullptr case
    if(str == nullptr) {
        *(uint16_t*)payload.data() = 0xffff;
        return;
    }

    std::string str_msg = str;
    writeString(str_msg);
}

void Message::readString(std::string& str) {
    str.clear();
    str.shrink_to_fit();

    uint16_t size_of_string = *((uint16_t*)(payload.data() + payload_read_bytes));
    if(size_of_string == 0xffff) {
        return;
    }

    str.resize(size_of_string);
    std::memcpy((void*)str.data(), (void*)(payload.data() + 2 + payload_read_bytes), size_of_string);
    payload_read_bytes += 2 + size_of_string;
}

void Message::SetMessageType(const MessageID& val) {
    message_type_index = val;
}

}