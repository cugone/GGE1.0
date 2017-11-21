#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <string>

namespace Net {

class Address;
class TCPSocket;

bool NetSystemStartup();
void NetSystemShutdown();

void SocketAddressFromNetAddress(sockaddr* out, unsigned int& addr_size, const Address& addr);
bool NetAddressFromSocketAddress(Address& out, sockaddr* socket_address);

void NetErrorToString(int error, std::string& error_str);
void PrintNetErrorString(int error, std::string msg);

}