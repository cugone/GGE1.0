#pragma once

#include <functional>
#include <array>

#include "Engine/Core/FileUtils.hpp"


namespace Net {

class Connection;

enum class MessageCoreID : unsigned char {
    MESSAGE_ID_JOIN_RESPONSE = 0,
};

enum class MessageID : unsigned char {
    MESSAGE_ID_PING = 1,
    MESSAGE_ID_PONG = 2,
    MESSAGE_ID_SEND_COMMAND = 32,
    MESSAGE_ID_SEND_MESSAGE = 33,
};

enum class MessageNetObjectID : unsigned char {
    MESSAGE_ID_NET_CREATE = 34,
    MESSAGE_ID_NET_DESTROY = 35,
    MESSAGE_ID_NET_UPDATE = 36,
};

enum class MessageRpcID : unsigned char {
    MESSAGE_ID_RPC_CALL = 37,
    MESSAGE_ID_RPC_RETURN = 38,
};

class Message : public FileUtils::BinaryStream {
public:

    Message(const MessageID& messageType);

    virtual ~Message() override;

    virtual std::size_t read_bytes(void* out_buffer, const std::size_t count) override;
    virtual std::size_t write_bytes(const void* buffer, const std::size_t size) const override;

    void writeString(const char* str);

    void writeString(const std::string& str);

    void readString(std::string& str);

    void SetMessageType(const Net::MessageID& val);
    const Net::MessageID& GetMessageType() const;

    Net::Connection* sender;
    std::array<unsigned char, 1024> payload;
    mutable std::size_t payload_read_bytes;
    mutable std::size_t payload_write_bytes;
private:
    MessageID message_type_index;
};

class MessageDefinition {
public:
    MessageID type_index;
    std::function<void(Net::Message*)> handler;
};

}