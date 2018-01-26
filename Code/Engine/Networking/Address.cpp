#include "Engine/Networking/Address.hpp"

#include <sstream>
#include <string>

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Networking/NetSystem.hpp"

namespace Net {

//Must match exactly "x[xx].x[xx].x[xx].x[xx]:x[xxxx]"
std::regex Address::ipport_regex(R"(([0-9]{1,3}\.){3}[0-9]{1,3}\:[0-9]{1,5})", std::regex::optimize);

Address::Address(const std::string& value)
    : address(0)
    , port(0)
    , address_v6{ 0 }
{
    *this = Net::NetAddressFromString(value);
}

Address::Address()
    : address(0)
    , port(0)
    , address_v6{ 0 }
{
    /* DO NOTHING */
}

bool Address::operator!=(const Address& other) const {
    return !(*this == other);
}

bool Address::operator==(const Address& other) const {
    return this->address == other.address && this->port == other.port;
}

std::vector<Net::Address> GetAddressesFromHostName(const char* hostname, unsigned short port, bool bindable) {

    std::string service = std::to_string(port);

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; //IPv4 and IPv6 interoperability
    hints.ai_socktype = SOCK_STREAM; //TCP = SOCK_STREAM; UDP = SOCK_DGRAM
    hints.ai_flags |= (bindable ? AI_PASSIVE : 0);
    //hints.ai_flags |= AI_NUMERICHOST; Use only if numeric IPs only.

    addrinfo* result = nullptr;
    int status = ::getaddrinfo(hostname, service.c_str(), &hints, &result);

    std::vector<Address> addresses;
    if(status != 0) {
        Net::PrintNetErrorString(status, "Net::GetAddressFromHostName failed. ");
        return addresses;
    }

    addrinfo* addr = result;
    while(addr != nullptr) {
        Address net_addr;
        if(Net::NetAddressFromSocketAddress(net_addr, addr->ai_addr)) {
            addresses.push_back(net_addr);
        }
        addr = addr->ai_next;
    }

    ::freeaddrinfo(result);

    return addresses;
}

std::vector<Net::Address> GetAddressesFromNetAddress(const Net::Address& addr, bool bindable) {
    return GetAddresses(addr.address, addr.port, bindable);
}

std::vector<Net::Address> GetAddresses(unsigned int addr, unsigned short port, bool bindable) {
    Net::Address net_addr;
    net_addr.useIpv6 = false;
    net_addr.address = addr;
    net_addr.port = port;
    const std::string& host = HostNameFromNetAddress(net_addr);
    return GetAddressesFromHostName(host.c_str(), net_addr.port, bindable);
}

std::vector<Net::Address> GetMyAddresses(unsigned short port) {

    std::size_t name_size = 256;
    std::vector<char> name_buffer;
    name_buffer.resize(name_size);
    auto ret_val = ::gethostname(name_buffer.data(), static_cast<int>(name_buffer.size()));
    std::string name(name_buffer.begin(), name_buffer.end());
    if(ret_val == 0) {
        name = name.substr(0, name.find_first_of('\0'));
        name.shrink_to_fit();
    }
    return GetAddressesFromHostName(name.c_str(), port, true);
}

std::string NetAddressToString(const Address& addr) {
    if(addr.useIpv6) {
        std::string result;
        std::ostringstream ss;
        for(unsigned short i : addr.address_v6) {
            ss << std::hex << i << ':';
        }
        unsigned short port = addr.port;
        result += ss.str() + std::to_string(port);
        return result;
    } else {
        std::string result;
        unsigned char A = (unsigned char)((addr.address >> 24) & 0xFF);
        unsigned char B = (unsigned char)((addr.address >> 16) & 0xFF);
        unsigned char C = (unsigned char)((addr.address >> 8) & 0xFF);
        unsigned char D = (unsigned char)((addr.address >> 0) & 0xFF);
        unsigned short port = addr.port;
        result += std::to_string((unsigned short)A) + std::string(".") + std::to_string((unsigned short)B) + std::string(".") + std::to_string((unsigned short)C) + std::string(".") + std::to_string((unsigned short)D) + std::string(":") + std::to_string(port);
        return result;
    }
}

Net::Address NetAddressFromString(const std::string& addrStr) {

    if(!std::regex_match(addrStr, Net::Address::ipport_regex)) {
        return Address{};
    }

    std::istringstream ss(addrStr);
    std::string addr_byte;

    unsigned char A = 0;
    unsigned char B = 0;
    unsigned char C = 0;
    unsigned char D = 0;
    unsigned short port = 0;

    unsigned char i = 0;
    //AAA.BBB.CCC.DDD:DDDD
    //std::sto* functions automatically stop at non-convertable chars based on locale.
    //Since ':' is never part of a number D will only contain last address byte before the port number.
    while(std::getline(ss, addr_byte, '.')) {
        switch(i++) {
            case 0: A = (unsigned char)std::stoul(addr_byte); break;
            case 1: B = (unsigned char)std::stoul(addr_byte); break;
            case 2: C = (unsigned char)std::stoul(addr_byte); break;
            case 3: D = (unsigned char)std::stoul(addr_byte); break;
        }
    }
    port = (unsigned short)std::stoul(addr_byte.substr(addr_byte.find_first_of(':') + 1));
    
    Address result;
    result.address = (A << 24) | (B << 16) | (C << 8) | (D << 0);
    result.port = port;
    return result;
}

std::string HostNameFromNetAddress(const Address& addr) {
    std::string host_name;
    unsigned char A = (unsigned char)((addr.address >> 24) & 0xFF);
    unsigned char B = (unsigned char)((addr.address >> 16) & 0xFF);
    unsigned char C = (unsigned char)((addr.address >> 8) & 0xFF);
    unsigned char D = (unsigned char)((addr.address >> 0) & 0xFF);
    host_name += std::to_string((unsigned short)A) + std::string(".") + std::to_string((unsigned short)B) + std::string(".") + std::to_string((unsigned short)C) + std::string(".") + std::to_string((unsigned short)D);
    return host_name;
}

}