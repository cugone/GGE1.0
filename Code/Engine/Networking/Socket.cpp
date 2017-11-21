#include "Engine/Networking/Socket.hpp"

namespace Net {

Socket::Socket()
    : my_socket(INVALID_SOCKET)
{
    /* DO NOTHING */
}
Socket::~Socket() {
    Close();
}

bool Socket::IsValid() const {
    return my_socket != INVALID_SOCKET;
}

void Socket::Close() {
    if(IsValid()) {
        ::closesocket(my_socket);
        my_socket = INVALID_SOCKET;
    }
}

void Socket::SetBlocking(bool blocking) {
    if(!IsValid()) {
        return;
    }
    unsigned long non_blocking = blocking ? 0 : 1;
    ::ioctlsocket(my_socket, FIONBIO, &non_blocking);
}

const Net::Address& Socket::GetAddress() const {
    return m_address;
}


}