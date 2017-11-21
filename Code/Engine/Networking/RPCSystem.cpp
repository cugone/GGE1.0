#include "Engine/Networking/RPCSystem.hpp"

#include <sstream>

#include "Engine/EngineConfig.hpp"

#include "Engine/Networking/Address.hpp"
#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/Connection.hpp"

namespace Net {

RPCSystem::RPCSystem(unsigned char max_connections /*= 32*/)
: session{}
, current_sender(nullptr)
{
    session.SetMaxConnectionCount(max_connections);
    SetupMessageDefinitions();
}

void RPCSystem::SetupMessageDefinitions() {
    session.RegisterMessageFunction(static_cast<Net::MessageID>(Net::MessageRpcID::MESSAGE_ID_RPC_CALL),
    [this](Net::Message* msg) {
        this->OnCallRPC(msg);
    });
    session.RegisterMessageFunction(static_cast<Net::MessageID>(Net::MessageRpcID::MESSAGE_ID_RPC_RETURN),
    [this](Net::Message* msg) {
        this->OnResultRPC(msg);
    });
}

void RPCSystem::Update(float /*deltaSeconds*/) {
    if(session.IsRunning()) {
        session.Update();
    } else {
        if(!session.Join(Net::GetMyAddresses(RPC_PORT)[0])) {
            session.Host(RPC_PORT);
            session.StartListening();
        }
    }
}

void RPCSystem::OnCallRPC(Net::Message* msg) {
    unsigned char source_conn_id;
    msg->read(source_conn_id);
    unsigned char target_conn_id;
    msg->read(target_conn_id);
    if(session.AmIHost()) {
        if(target_conn_id != session.my_connection->connection_index) {
            //Copy message and mark as unread.
            Net::Message client_msg = Net::Message(static_cast<Net::MessageID>(Net::MessageRpcID::MESSAGE_ID_RPC_CALL));
            client_msg = *msg;
            client_msg.payload_read_bytes = 0;
            session.SendMessageToOther(target_conn_id, client_msg);
        } else {
            std::string id;
            msg->readString(id);

            auto entry_iter = _rpcs.find(id);
            if(entry_iter == _rpcs.end()) {
                return;
            }
            auto entry = entry_iter->second;
            entry.deserialize_ptr(entry, *msg);

        }
    } else if(session.AmIClient()) {
        std::string id;
        msg->readString(id);

        auto entry_iter = _rpcs.find(id);
        if(entry_iter == _rpcs.end()) {
            return;
        }
        auto entry = entry_iter->second;
        entry.deserialize_ptr(entry, *msg);
    }
}

void SendRPC(Message& /*msg*/, unsigned char /*source*/, unsigned char /*target*/, const std::string& /*id*/) {
    /* DO NOTHING */
}

//Single-argument of type std::string overload.
template<>
void UnSendRPC_helper(Net::Message& msg, std::string& arg) {
    msg.readString(arg);
}

//---
//Single-argument of type std::string overload.
template<>
void SendRPC_helper(Net::Message& msg, std::string arg) {
    msg.writeString(arg);
}

//Single-argument of type const char* overload.
template<>
void SendRPC_helper(Net::Message& msg, const char* arg) {
    msg.writeString(arg);
}

void RPCSystem::OnResultRPC(Net::Message* msg) {
    unsigned char sender_id;
    msg->read(sender_id);
    int result;
    msg->read(result);
    std::ostringstream ss;
    ss << "RPC returned: Sender: " << sender_id << " Result: " << result;
    g_theConsole->NotifyMsg(ss.str());
}

} //End Net