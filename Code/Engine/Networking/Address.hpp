#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <regex>
#include <string>
#include <vector>

namespace Net {

class Address {
public:
    Address();
    ~Address() = default;
    Address(const Address& other) = default;
    Address& operator=(const Net::Address& rhs) = default;

    explicit Address(const std::string& value);

    //MUTUALLY EXCLUSIVE
    unsigned int address;
    unsigned short address_v6[8];

    unsigned short port;

    bool operator==(const Net::Address& other) const;
    bool operator!=(const Net::Address& other) const;

    bool useIpv6 = false;

protected:
private:
    static std::regex ipport_regex;
    friend Net::Address NetAddressFromString(const std::string& addrStr);
};

std::vector<Net::Address> GetAddressesFromNetAddress(const Net::Address& addr, bool bindable);
std::vector<Net::Address> GetAddressesFromHostName(const char* hostname, unsigned short port, bool bindable);
std::vector<Net::Address> GetAddresses(unsigned int addr, unsigned short port, bool bindable);
std::vector<Net::Address> GetMyAddresses(unsigned short port);
std::string NetAddressToString(const Net::Address& addr);
Net::Address NetAddressFromString(const std::string& addrStr);
std::string HostNameFromNetAddress(const Net::Address& addr);

}