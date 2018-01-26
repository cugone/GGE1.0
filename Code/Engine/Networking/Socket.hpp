#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

#include "Engine/Networking/Address.hpp"

namespace Net {

class Address;

class Socket {
public:
    Socket();
    ~Socket();

    virtual std::size_t Send(const void* payload, std::size_t size) = 0;
    virtual std::size_t Receive(void* payload, std::size_t max_size) = 0;
    virtual bool CheckForDisconnect() = 0;

    virtual bool IsValid() const;
    virtual void SetBlocking(bool blocking);
    virtual void Close();

    const Net::Address& GetAddress() const;

protected:
    Net::Address m_address;
    SOCKET my_socket;
private:

};

}