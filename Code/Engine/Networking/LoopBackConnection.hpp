#pragma once

#include <queue>

#include "Engine/Networking/Connection.hpp"

namespace Net {

class Message;

class LoopBackConnection : public Net::Connection {
public:
    virtual ~LoopBackConnection();

    virtual void Send(Net::Message* msg) override;

    virtual void Receive(Net::Message** msg) override;

    std::queue<Net::Message*> message_queue;
};

}