#pragma once

#include "Engine/Networking/Address.hpp"

namespace Net {

class Message;
class Session;

class Connection {
public:
    virtual ~Connection() = default;

    virtual void Send(Net::Message* msg) = 0;
    virtual void Receive(Net::Message** msg) = 0;

    Net::Session* owner;

    Net::Address address;
    unsigned char connection_index; //LUID
};

}