#pragma once

#include "Engine/Networking/Session.hpp"

namespace Net {

class TCPSocket;
class Address;
class Message;

class TCPSession : public Session {
public:
    TCPSession();
    virtual ~TCPSession() = default;

    //Create a loopback - add it to connections, set my and host connection to it.
    virtual bool Host(unsigned short port) override;

    //Attempt to join the supplied address.
    virtual bool Join(const Net::Address& addr) override;

    //If I have a connection - destroy it.
    virtual void Leave() override;

    //Process all connections.
    virtual void Update() override;

    virtual bool StartListening() override;
    virtual void StopListening() override;
    virtual bool IsListening() override;

    void SendJoinInfo(Net::Connection* cp);
    void OnJoinResponse(Net::Message* msg);

    std::vector<TCPSocket*> listen_sockets;

};

}