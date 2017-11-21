#include "Engine/Networking/LoopBackConnection.hpp"

namespace Net {

LoopBackConnection::~LoopBackConnection() {
    while(!message_queue.empty()) {
        message_queue.pop();
    }
}

void Net::LoopBackConnection::Send(Net::Message* msg) {
    message_queue.push(msg);
}

void LoopBackConnection::Receive(Net::Message** msg) {
    if(message_queue.empty()) {
        *msg = nullptr;
        return;
    }
    *msg = message_queue.front();
    message_queue.pop();
}
}