#include "Engine/Networking/TCPSession.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Networking/Connection.hpp"
#include "Engine/Networking/TCPConnection.hpp"
#include "Engine/Networking/LoopBackConnection.hpp"
#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/Socket.hpp"
#include "Engine/Networking/TCPSocket.hpp"

namespace Net {

TCPSession::TCPSession()
: Session()
, listen_sockets{}
{
    RegisterMessageFunction(static_cast<Net::MessageID>(Net::MessageCoreID::MESSAGE_ID_JOIN_RESPONSE),
                                     [this](Net::Message* msg) { this->OnJoinResponse(msg); });
}

bool Net::TCPSession::Host(unsigned short port) {
    ASSERT_OR_DIE(!IsRunning(), "TCPSession::ctor: Must not already be running to host connection.");
    my_connection = new LoopBackConnection;
    auto my_addrs = GetMyAddresses(port);
    listen_sockets.resize(my_addrs.size());
    my_connection->address = my_addrs[0];
    my_connection->owner = this;
    JoinConnection(0, my_connection);
    host_connection = my_connection;
    SetState(SessionState::READY);
    return true;
}

bool TCPSession::Join(const Net::Address& addr) {

    TCPConnection* host = new TCPConnection;
    host->address = addr;
    host->owner = this;

    host->socket = new TCPSocket;
    if(!host->Connect()) {
        Leave();
        //Leave deletes host;
        host = nullptr;
        return false;
    }

    host->socket->SetBlocking(false);

    JoinConnection(0, host); //0 for this class

    host_connection = host;
    my_connection = new LoopBackConnection;
    my_connection->address = GetMyAddresses(addr.port)[0];
    my_connection->owner = this;
    SetState(SessionState::CONNECTING);
    return true;
}

void TCPSession::Leave() {
    DestroyConnection(my_connection);
    DestroyConnection(host_connection);
    for(auto& connection : connections) {
        DestroyConnection(connection);
    }
    connections.clear();
    StopListening();
    SetState(SessionState::DISCONNECTED);
}

void TCPSession::Update() {

    if(IsListening()) {
        std::vector<TCPSocket*> sockets(listen_sockets.size());
        for(std::size_t i = 0; i < sockets.size(); ++i) {

            if(listen_sockets[i] == nullptr) {
                continue;
            }

            auto& cur_socket = sockets[i];
            cur_socket = listen_sockets[i]->Accept();
            if(cur_socket == nullptr) {
                continue;
            }
            cur_socket->SetBlocking(false);

            TCPConnection* new_guy = new TCPConnection;
            new_guy->address = cur_socket->GetAddress();
            new_guy->socket = cur_socket;
            new_guy->owner = this;
            uint8_t conn_idx = GetFreeConnectionIndex();
            if(conn_idx == INVALID_CONNECTION_INDEX) {
                delete new_guy;

                //TODO: MORE TO DO
                continue;
            }
            new_guy->connection_index = conn_idx;
            new_guy->owner->SetState(Net::SessionState::READY);
            JoinConnection(new_guy->connection_index, new_guy);
            SendJoinInfo(new_guy);

        }
    }

    for(std::size_t i = 0; i < connection_list.size(); ++i) {
        if(!connections[i]) {
            continue;
        }

        Net::Message* msg = nullptr;
        connections[i]->Receive(&msg);
        while(msg) {
            msg->sender = connections[i];
            ProcessMessage(msg);
            delete msg;
            msg = nullptr;
            if(connections[i]) {
                connections[i]->Receive(&msg);
            }
        }
    }

    for(std::size_t i = 0; i < connection_list.size(); ++i) {
        if(connections[i] == nullptr || connections[i] == my_connection) {
            continue;
        }
        TCPConnection* tcp_connection = dynamic_cast<Net::TCPConnection*>(connections[i]);
        if(tcp_connection && tcp_connection->IsDisconnected()) {
            DestroyConnection(tcp_connection);
            tcp_connection = nullptr;
        }
    }

    if(host_connection == nullptr) {
        Leave();
    }

}

void TCPSession::SendJoinInfo(Net::Connection* cp) {
    Net::Message* msg = new Net::Message(static_cast<Net::MessageID>(Net::MessageCoreID::MESSAGE_ID_JOIN_RESPONSE));
    msg->write(cp->connection_index);
    cp->Send(msg);
}

void TCPSession::OnJoinResponse(Net::Message* msg) {
    uint8_t my_conn_index;
    msg->read(my_conn_index);

    JoinConnection(my_conn_index, my_connection);
    SetState(SessionState::READY);

}

bool TCPSession::StartListening() {
    if(!AmIHost()) {
        return false;
    }
    if(IsListening()) {
        return true;
    }

    for(auto& ls : listen_sockets) {
        TCPSocket* listen_socket = new TCPSocket;
        if(listen_socket->Listen(my_connection->address.port)) {
            listen_socket->SetBlocking(false);
            //Store valid socket into array
            if(!ls) {
                ls = listen_socket;
            }
        } else {
            delete listen_socket;
            listen_socket = nullptr;
        }
    }
    return IsListening();
}

void TCPSession::StopListening() {
    if(IsListening()) {
        for(auto& listen_socket : listen_sockets) {
            if(listen_socket) {
                delete listen_socket;
                listen_socket = nullptr;
            }
        }
    }
}

bool TCPSession::IsListening() {
    for(auto listen_socket : listen_sockets) {
        if(listen_socket) {
            return true;
        }
    }
    return false;
}

}