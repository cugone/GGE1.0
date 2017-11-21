#pragma once

#include "Engine/Networking/Socket.hpp"

namespace Net {

class TCPSocket : public Socket {
public:
    TCPSocket();
    ~TCPSocket();

    //CLIENT
    bool Join(const Address& addr);

    bool Listen(unsigned short port, unsigned int max_queue_size = 8);
    bool IsListening() const;

    //BOTH
    TCPSocket* Accept();

    virtual int Send(const void* payload, unsigned int size) override;
    virtual unsigned int Receive(void* payload, unsigned int max_size) override;

    virtual bool CheckForDisconnect() override;

protected:
private:
    bool m_isListenSocket;
};

}