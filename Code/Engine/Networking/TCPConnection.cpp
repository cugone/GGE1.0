#include "Engine/Networking/TCPConnection.hpp"

#include <type_traits>

#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/TCPSocket.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

namespace Net {

TCPConnection::TCPConnection()
    : Net::Connection()
    , socket(nullptr)
{
    /* DO NOTHING */
}

TCPConnection::~TCPConnection() {
    /* DO NOTHING */
}

void TCPConnection::Send(Net::Message* msg) {
    DebuggerPrintf("Sent: %u\n", msg->GetMessageType());
    uint16_t sz = (uint16_t)(msg->payload_write_bytes + 1);
    socket->Send(&sz, 2);
    socket->Send(&msg->GetMessageType(), 1);
    socket->Send(msg->payload.data(), msg->payload_write_bytes);

}

void TCPConnection::Receive(Net::Message** msg) {
    if(rbtm < 2) {
        std::size_t recv_cnt = socket->Receive(recv_buffer.data() + rbtm, 2 - rbtm);
        if(recv_cnt == 0) {
            return;
        }
        rbtm += recv_cnt;
    } else {
        uint16_t bytes_to_read = *(uint16_t*)recv_buffer.data();
        if(rbtm < 2u + bytes_to_read) {
            std::size_t recv_cnt = socket->Receive(recv_buffer.data() + rbtm, (2u + bytes_to_read) - rbtm);
            if(recv_cnt == 0) {
                return;
            }
            rbtm += recv_cnt;
            if(rbtm >= 2u + bytes_to_read) {
                *msg = new Message(static_cast<Net::MessageID>(recv_buffer.data()[2]));
                DebuggerPrintf("Received: %u\n", (*msg)->GetMessageType());
                std::vector<unsigned char> buf;
                buf.resize(bytes_to_read - 1);
                std::memcpy(buf.data(), recv_buffer.data() + 3, bytes_to_read - 1);
                (*msg)->write_bytes(buf.data(), buf.size());
                rbtm = 0;
            }
        }
    }
}

bool TCPConnection::Connect() {
    return socket->Join(address);
}

bool TCPConnection::IsDisconnected() {
    if((socket == nullptr) || (socket && !socket->IsValid())) {
        return true;
    }
    return false;
}

}