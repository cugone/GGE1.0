#pragma once

#include <array>

#include "Engine/Networking/Connection.hpp"

namespace Net {

class Message;
class MessageDefinition;
class Session;
class Connection;
class TCPConnection;
class TCPSession;
class TCPSocket;

class TCPConnection : public Net::Connection {

public:
    TCPConnection();
    virtual ~TCPConnection() override;

    virtual void Send(Net::Message* msg) override;
    virtual void Receive(Net::Message** msg) override;

    bool Connect();
    bool IsDisconnected();

    TCPSocket* socket;
protected:
private:
    std::array<unsigned char, 1027> recv_buffer = { 0 };
    std::size_t rbtm = 0;
};

}