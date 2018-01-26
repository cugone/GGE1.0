#include "Engine/Networking/TCPSocket.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Networking/Address.hpp"
#include "Engine/Networking/NetSystem.hpp"

namespace Net {

TCPSocket::TCPSocket()
: Net::Socket()
, m_isListenSocket(false)
{ /* DO NOTHING */ }

TCPSocket::~TCPSocket()
{
    if(IsValid()) {
        m_isListenSocket = false;
    }
}

bool TCPSocket::Join(const Address& addr) {
    if(IsValid()) {
        return false;
    }

    SOCKET sock = INVALID_SOCKET;
    if(addr.useIpv6) {
        sock = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET) {
            return false;
        }
    } else {
        sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET) {
            return false;
        }
    }

    sockaddr_storage internal_address;
    unsigned int addr_size = 0;
    Net::SocketAddressFromNetAddress((sockaddr*)&internal_address, addr_size, addr);
    
    int result = ::connect(sock, (sockaddr*)&internal_address, addr_size);
    if(result != 0) {
        int error = ::WSAGetLastError();
        PrintNetErrorString(error, "TCPSocket::Join failed. ");
        Close();
        return false;
    } else {
        my_socket = sock;
        m_address = addr;
        return true;
    }

}

Net::TCPSocket* TCPSocket::Accept() {
    if(!m_isListenSocket) {
        return nullptr;
    }

    sockaddr_storage conn_addr;
    int conn_addr_len = sizeof(conn_addr);
    SOCKET their_socket = ::accept(my_socket, (sockaddr*)&conn_addr, &conn_addr_len);
    if(their_socket == INVALID_SOCKET) {
        return nullptr;
    }

    Net::Address net_addr;
    if(!Net::NetAddressFromSocketAddress(net_addr, (sockaddr*)&conn_addr)) {
        ::closesocket(their_socket);
        return nullptr;
    }

    TCPSocket* their_tcp_socket = new TCPSocket;
    their_tcp_socket->my_socket = their_socket;
    their_tcp_socket->m_address = net_addr;
    their_tcp_socket->SetBlocking(false);
    return their_tcp_socket;

}

std::size_t TCPSocket::Send(const void* payload, std::size_t size) {
    
    if(!IsValid()) {
        return 0;
    }

    if(m_isListenSocket) {
        return 0;
    }

    int bytes_sent = ::send(my_socket, (const char*)payload, (int)size, 0);
    if(bytes_sent <= 0) {
        int error = WSAGetLastError();
        PrintNetErrorString(error, "TCPSocket::Send failed. ");
        Close();
        return 0;
    }

    ASSERT_OR_DIE(bytes_sent == (int)size, "TCPSocket::send assert failed. Bytes sent does not match parameter.\n");
    return bytes_sent;

}

std::size_t TCPSocket::Receive(void* payload, std::size_t max_size) {
    if(!IsValid() || max_size == 0 || payload == nullptr) {
        return 0;
    }

    if(m_isListenSocket) {
        return 0;
    }

    int bytes_read = ::recv(my_socket, (char*)payload, (int)max_size, 0);
    if(bytes_read <= 0) {
        if(bytes_read == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if(error == WSAEWOULDBLOCK) {
                return 0;
            } else {
                PrintNetErrorString(error, "TCPSocket::Receive failed. ");
                Close();
                return 0;
            }
        } else {
            if(CheckForDisconnect()) {
                return 0;
            }
        }
        return 0;
    } else {
        return (unsigned int)bytes_read;
    }

}

bool TCPSocket::IsListening() const {
    return m_isListenSocket;
}

bool TCPSocket::CheckForDisconnect() {

    if(!IsValid()) {
        return true;
    }

    WSAPOLLFD fd;
    fd.fd = my_socket;
    fd.events = POLLRDNORM;

    if(SOCKET_ERROR == ::WSAPoll(&fd, 1, 0)) {
        return true;
    }

    if((fd.revents & POLLHUP) != 0) {
        Close();
        return true;
    }

    return false;

}

bool TCPSocket::Listen(unsigned short port, unsigned int max_queue_size /*= 8*/) {
    if(IsValid()) {
        return false;
    }

    std::vector<Net::Address> addresses = Net::GetMyAddresses(port);
    if(addresses.empty()) {
        return false;
    }

    SOCKET listen_socket = ::socket(addresses[0].useIpv6 ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(listen_socket == INVALID_SOCKET) {
        return false;
    }

    sockaddr_storage listen_address;
    unsigned int addr_size = 0;
    Net::SocketAddressFromNetAddress((sockaddr*)&listen_address, addr_size, addresses[0]);

    //Associate address to this socket.
    int result = ::bind(listen_socket, (sockaddr*)&listen_address, (int)addr_size);
    if(result == SOCKET_ERROR) {
        ::closesocket(listen_socket);
        return false;
    }

    result = ::listen(listen_socket, (int)max_queue_size);
    if(result == SOCKET_ERROR) {
        ::closesocket(listen_socket);
        return false;
    }

    my_socket = listen_socket;
    m_isListenSocket = true;
    m_address = addresses[0];
    return true;
}

}