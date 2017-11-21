#include "Engine/Networking/NetSystem.hpp"

#include <string>

#include "Engine/EngineConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Networking/Address.hpp"
#include "Engine/Networking/TCPSocket.hpp"

namespace Net {

bool NetSystemStartup() {

    g_theFileLogger->LogPrintf("Initializing networking system.\n");
    WORD version = MAKEWORD(2, 2);

    WSADATA data;
    int error = ::WSAStartup(version, &data);
    bool success = error == 0;
    if(!success) {
        PrintNetErrorString(error, "Error initializing networking system. ");
        return false;
    }
    return success;
}

void NetSystemShutdown() {
    g_theFileLogger->LogPrintf("Shutting down networking system.\n");
    ::WSACleanup();
}

void SocketAddressFromNetAddress(sockaddr* out, unsigned int& addr_size, const Address& addr) {

    if(addr.useIpv6) {
        sockaddr_in6* out_addr = (sockaddr_in6*)out;
        memset(out_addr, 0, sizeof(sockaddr_in6));

        out_addr->sin6_family = PF_INET6;
        out_addr->sin6_port = ::htons(addr.port);
        IN6_ADDR addr6;
        memset(&addr6, 0, sizeof(IN6_ADDR));

        for(std::size_t i = 0; i < 8; ++i) {
            addr6.u.Word[i] = ::htons(addr.address_v6[i]);
        }

        out_addr->sin6_addr = addr6;

        addr_size = sizeof(sockaddr_in6);
    } else {
        sockaddr_in* out_addr = (sockaddr_in*)out;
        memset(out_addr, 0, sizeof(sockaddr_in));

        out_addr->sin_family = PF_INET;
        out_addr->sin_port = ::htons(addr.port);
        out_addr->sin_addr.S_un.S_addr = htonl(addr.address);

        addr_size = sizeof(sockaddr_in);
    }
}

bool NetAddressFromSocketAddress(Address& out, sockaddr* socket_address) {

    if(socket_address == nullptr) {
        return false;
    }
    bool isIpv4 = socket_address->sa_family == AF_INET;
    bool isIpv6 = socket_address->sa_family == AF_INET6;
    bool isValidFamily = isIpv4 || isIpv6;
    if(!isValidFamily) {
        return false;
    }

    if(isIpv4) {
        sockaddr_in* ipv4_address = (sockaddr_in*)socket_address;
        out.useIpv6 = false;
        out.port = ::ntohs(ipv4_address->sin_port);
        out.address = ::ntohl(ipv4_address->sin_addr.S_un.S_addr);
    } else if(isIpv6) {
        sockaddr_in6* ipv6_address = (sockaddr_in6*)socket_address;
        out.useIpv6 = true;
        out.port = ::ntohs(ipv6_address->sin6_port);
        for(int i = 0; i < 8; ++i) {
            out.address_v6[i] = ::ntohs(ipv6_address->sin6_addr.u.Word[i]);
        }
    }
    return true;
}

void PrintNetErrorString(int error, std::string msg) {
    std::string error_str;
    Net::NetErrorToString(error, error_str);
    msg += "Error: %s\n";
    DebuggerPrintf(msg.c_str(), error_str.c_str());
}


void NetErrorToString(int error, std::string& error_str) {
    switch(error) {
        case WSA_INVALID_HANDLE:
            error_str = "Specified event object handle is invalid.";
            break;
        case WSA_NOT_ENOUGH_MEMORY:
            error_str = "Insufficient memory available.";
            break;
        case WSA_INVALID_PARAMETER:
            error_str = "One or more parameters are invalid.";
            break;
        case WSA_OPERATION_ABORTED:
            error_str = "Overlapped operation aborted.";
            break;
        case WSA_IO_INCOMPLETE:
            error_str = "Overlapped I/O event object not in signaled state.";
            break;
        case WSA_IO_PENDING:
            error_str = "Overlapped operations will complete later.";
            break;
        case WSAEINTR:
            error_str = "Interrupted function call.";
            break;
        case WSAEBADF:
            error_str = "File handle is not valid.";
            break;
        case WSAEACCES:
            error_str = "Permission denied.";
            break;
        case WSAEFAULT:
            error_str = "Bad address.";
            break;
        case WSAEINVAL:
            error_str = "Invalid argument.";
            break;
        case WSAEMFILE:
            error_str = "Too many open files.";
            break;
        case WSAEWOULDBLOCK:
            error_str = "Resource temporarily unavailable.";
            break;
        case WSAEINPROGRESS:
            error_str = "Operation now in progress.";
            break;
        case WSAEALREADY:
            error_str = "Operation already in progress.";
            break;
        case WSAENOTSOCK:
            error_str = "Socket operation on nonsocket.";
            break;
        case WSAEDESTADDRREQ:
            error_str = "Destination address required.";
            break;
        case WSAEMSGSIZE:
            error_str = "Message too long.";
            break;
        case WSAEPROTOTYPE:
            error_str = "Protocol wrong type for socket.";
            break;
        case WSAENOPROTOOPT:
            error_str = "Bad protocol option.";
            break;
        case WSAEPROTONOSUPPORT:
            error_str = "Protocol not supported.";
            break;
        case WSAESOCKTNOSUPPORT:
            error_str = "Socket type not supported.";
            break;
        case WSAEOPNOTSUPP:
            error_str = "Operation not supported.";
            break;
        case WSAEPFNOSUPPORT:
            error_str = "Protocol family not supported.";
            break;
        case WSAEAFNOSUPPORT:
            error_str = "Address family not supported by protocol family.";
            break;
        case WSAEADDRINUSE:
            error_str = "Address already in use.";
            break;
        case WSAEADDRNOTAVAIL:
            error_str = "Cannot assign requested address.";
            break;
        case WSAENETDOWN:
            error_str = "Network is down.";
            break;
        case WSAENETUNREACH:
            error_str = "Network is unreachable.";
            break;
        case WSAENETRESET:
            error_str = "Network dropped connection on reset.";
            break;
        case WSAECONNABORTED:
            error_str = "Software caused connection abort.";
            break;
        case WSAECONNRESET:
            error_str = "Connection reset by peer.";
            break;
        case WSAENOBUFS:
            error_str = "No buffer space available.";
            break;
        case WSAEISCONN:
            error_str = "Socket is already connected.";
            break;
        case WSAENOTCONN:
            error_str = "Socket is not connected.";
            break;
        case WSAESHUTDOWN:
            error_str = "Cannot send after socket shutdown.";
            break;
        case WSAETOOMANYREFS:
            error_str = "Too many references.";
            break;
        case WSAETIMEDOUT:
            error_str = "Connection timed out.";
            break;
        case WSAECONNREFUSED:
            error_str = "Connection refused.";
            break;
        case WSAELOOP:
            error_str = "Cannot translate name.";
            break;
        case WSAENAMETOOLONG:
            error_str = "Name too long.";
            break;
        case WSAEHOSTDOWN:
            error_str = "Host is down.";
            break;
        case WSAEHOSTUNREACH:
            error_str = "No route to host.";
            break;
        case WSAENOTEMPTY:
            error_str = "Directory not empty.";
            break;
        case WSAEPROCLIM:
            error_str = "Too many processes.";
            break;
        case WSAEUSERS:
            error_str = "User quota exceeded.";
            break;
        case WSAEDQUOT:
            error_str = "Disk quota exceeded.";
            break;
        case WSAESTALE:
            error_str = "Stale file handle reference.";
            break;
        case WSAEREMOTE:
            error_str = "Item is remote.";
            break;
        case WSASYSNOTREADY:
            error_str = "Network subsystem is unavailable.";
            break;
        case WSAVERNOTSUPPORTED:
            error_str = "Winsock.dll version out of range.";
            break;
        case WSANOTINITIALISED:
            error_str = "Successful WSAStartup not yet performed.";
            break;
        case WSAEDISCON:
            error_str = "Graceful shutdown in progress.";
            break;
        case WSAENOMORE:
            error_str = "No more results.";
            break;
        case WSAECANCELLED:
            error_str = "Call has been canceled.";
            break;
        case WSAEINVALIDPROCTABLE:
            error_str = "Procedure call table is invalid.";
            break;
        case WSAEINVALIDPROVIDER:
            error_str = "Service provider is invalid.";
            break;
        case WSAEPROVIDERFAILEDINIT:
            error_str = "Service provider failed to initialize.";
            break;
        case WSASYSCALLFAILURE:
            error_str = "System call failure.";
            break;
        case WSASERVICE_NOT_FOUND:
            error_str = "Service not found.";
            break;
        case WSATYPE_NOT_FOUND:
            error_str = "Class type not found.";
            break;
        case WSA_E_NO_MORE:
            error_str = "No more results.";
            break;
        case WSA_E_CANCELLED:
            error_str = "Call was canceled.";
            break;
        case WSAEREFUSED:
            error_str = "Database query was refused.";
            break;
        case WSAHOST_NOT_FOUND:
            error_str = "Host not found.";
            break;
        case WSATRY_AGAIN:
            error_str = "Nonauthoritative host not found. Try again.";
            break;
        case WSANO_RECOVERY:
            error_str = "This is a nonrecoverable error.";
            break;
        case WSANO_DATA:
            error_str = "Valid name, no data record of requested type.";
            break;
        case WSA_QOS_RECEIVERS:
            error_str = "QoS receivers.";
            break;
        case WSA_QOS_SENDERS:
            error_str = "QoS senders.";
            break;
        case WSA_QOS_NO_SENDERS:
            error_str = "No QoS senders.";
            break;
        case WSA_QOS_NO_RECEIVERS:
            error_str = "QoS no receivers.";
            break;
        case WSA_QOS_REQUEST_CONFIRMED:
            error_str = "QoS request confirmed.";
            break;
        case WSA_QOS_ADMISSION_FAILURE:
            error_str = "QoS admission error.";
            break;
        case WSA_QOS_POLICY_FAILURE:
            error_str = "QoS policy failure.";
            break;
        case WSA_QOS_BAD_STYLE:
            error_str = "QoS bad style.";
            break;
        case WSA_QOS_BAD_OBJECT:
            error_str = "QoS bad object.";
            break;
        case WSA_QOS_TRAFFIC_CTRL_ERROR:
            error_str = "QoS traffic control error.";
            break;
        case WSA_QOS_GENERIC_ERROR:
            error_str = "QoS generic error.";
            break;
        case WSA_QOS_ESERVICETYPE:
            error_str = "QoS service type error.";
            break;
        case WSA_QOS_EFLOWSPEC:
            error_str = "QoS flowspec error.";
            break;
        case WSA_QOS_EPROVSPECBUF:
            error_str = "Invalid QoS provider buffer.";
            break;
        case WSA_QOS_EFILTERSTYLE:
            error_str = "Invalid QoS filter style.";
            break;
        case WSA_QOS_EFILTERTYPE:
            error_str = "Invalid QoS filter type.";
            break;
        case WSA_QOS_EFILTERCOUNT:
            error_str = "Incorrect QoS filter count.";
            break;
        case WSA_QOS_EOBJLENGTH:
            error_str = "Invalid QoS object length.";
            break;
        case WSA_QOS_EFLOWCOUNT:
            error_str = "Incorrect QoS flow count.";
            break;
        case WSA_QOS_EUNKOWNPSOBJ:
            error_str = "Unrecognized QoS object.";
            break;
        case WSA_QOS_EPOLICYOBJ:
            error_str = "Invalid QoS policy object.";
            break;
        case WSA_QOS_EFLOWDESC:
            error_str = "Invalid QoS flow descriptor.";
            break;
        case WSA_QOS_EPSFLOWSPEC:
            error_str = "Invalid QoS provider-specific flowspec.";
            break;
        case WSA_QOS_EPSFILTERSPEC:
            error_str = "Invalid QoS provider-specific filterspec.";
            break;
        case WSA_QOS_ESDMODEOBJ:
            error_str = "Invalid QoS shape discard mode object.";
            break;
        case WSA_QOS_ESHAPERATEOBJ:
            error_str = "Invalid QoS shaping rate object.";
            break;
        case WSA_QOS_RESERVED_PETYPE:
            error_str = "Reserved policy QoS element type.";
            break;
    }
}


}