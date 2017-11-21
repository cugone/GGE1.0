#pragma once

#include <string>
#include <functional>
#include <tuple>
#include <future>
#include <map>
#include <type_traits>
#include <utility>

#include "Engine/Networking/TCPSession.hpp"
#include "Engine/Networking/Connection.hpp"
#include "Engine/Networking/Message.hpp"

namespace Net {

class Connection;
struct RpcEntry;
class RPCSystem;

constexpr unsigned short RPC_PORT = 0x379F;//14239;

struct RpcEntry {
    std::string id;
    unsigned char source;
    unsigned char target;
    void* func_ptr;
    Net::Message(*serialize_ptr)(const RpcEntry& reg, void* params);
    void(*deserialize_ptr)(const RpcEntry& reg, Net::Message& msg);
};

template<typename Ret, typename... Args>
Net::Message RpcSerializeFunc(const RpcEntry& reg, void* params) {
    auto arguments = reinterpret_cast<std::tuple<Args...>*>(params);
    Net::Message send_rpc = Net::Message(static_cast<Net::MessageID>(Net::MessageRpcID::MESSAGE_ID_RPC_CALL));
    SendRPC_helper(send_rpc, reg.source, reg.target, reg.id, *arguments);
    return send_rpc;
}

template<typename Ret, typename... Args>
void RpcDeserializeFunc(const RpcEntry& reg, Net::Message& msg) {
    Ret(*cb)(Args...) = reinterpret_cast<Ret(*)(Args...)>(reg.func_ptr);
    std::tuple<Args...> arguments;
    UnSendRPC_helper(msg, arguments);
    std::apply(cb, arguments);
}

template<typename T>
void UnSendRPC_helper(Net::Message& /*msg*/) {
    /* DO NOTHING */
}

template<typename Arg, typename... Args>
void UnSendRPC_helper(Net::Message& msg, Arg& arg1, Args&... args) {
    UnSendRPC_helper(msg, arg1);
    UnSendRPC_helper(msg, args...);
}

//Single-argument overload
template<typename T>
void UnSendRPC_helper(Net::Message& msg, T& arg) {
    msg.read<T>(arg);
}

template <class Tuple, std::size_t... I>
constexpr decltype(auto) UnSendRPC_helper(Net::Message& msg, Tuple&& t, std::index_sequence<I...>) {
    return UnSendRPC_helper(msg, std::get<I>(std::forward<Tuple>(t))...);
}

//Single-argument overload
template<typename... Args>
void UnSendRPC_helper(Net::Message& msg, std::tuple<Args...>& arg) {
    UnSendRPC_helper(msg, arg, std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>{});
}

//Single-argument of type std::string overload.
template<>
void UnSendRPC_helper(Net::Message& msg, std::string& arg);

//-----
template<typename T>
void SendRPC_helper(Net::Message& /*msg*/) {
    /* DO NOTHING */
}

template<typename Arg, typename... Args>
void SendRPC_helper(Net::Message& msg, Arg arg1, Args... args) {
    SendRPC_helper(msg, arg1);
    SendRPC_helper(msg, args...);
}

//Single-argument overload
template<typename T>
void SendRPC_helper(Net::Message& msg, T arg) {
    msg.write<T>(arg);
}

template <class Tuple, std::size_t... I>
constexpr decltype(auto) SendRPC_helper(Net::Message& msg, Tuple&& t, std::index_sequence<I...>) {
    return SendRPC_helper(msg, std::get<I>(std::forward<Tuple>(t))...);
}

//Single-argument overload
template<typename... Args>
void SendRPC_helper(Net::Message& msg, std::tuple<Args...> arg) {
    SendRPC_helper(msg, arg, std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>{});
}

//Single-argument of type std::string overload.
template<>
void SendRPC_helper(Net::Message& msg, std::string arg);

//Single-argument of type const char* overload.
template<>
void SendRPC_helper(Net::Message& msg, const char* arg);

void SendRPC(Net::Message& /*msg*/, unsigned char /*source*/, unsigned char /*target*/, const std::string& /*id*/);

template<typename Arg, typename... Args>
void SendRPC(Net::Message& msg, unsigned char source, unsigned char target, const std::string& id, Arg arg1, Args... args) {

    msg.write(source);
    msg.write(target);
    msg.writeString(id);
    SendRPC_helper(msg, arg1);
    SendRPC_helper(msg, args...);

}

class RPCSystem {
public:

    RPCSystem(unsigned char max_connections = 32);
    ~RPCSystem() = default;

    void SetupMessageDefinitions();
    void Update(float deltaSeconds);

    Net::TCPSession session;
    Net::Connection* current_sender;

    template<typename Ret, typename... Args>
    void Register(const std::string& id, Ret(*function)(Args...));

    template<typename Ret, typename... Args>
    void Register(const char* id, Ret(*function)(Args...));

    template<typename Ret, typename... Args>
    void CallRPC(uint8_t source_id, uint8_t target_id, const std::string& id, Args... args);

    void OnCallRPC(Net::Message* msg);
    void OnResultRPC(Net::Message* msg);

protected:
private:

    template<typename Ret, typename... Args>
    void UnCallRPC(Net::Message& msg, RpcEntry& entry, Args... args);

    std::map<std::string, Net::RpcEntry> _rpcs = std::map<std::string, Net::RpcEntry>{};

};

template<typename Ret, typename... Args>
void RPCSystem::Register(const std::string& id, Ret(*function)(Args...)) {
    if(id.empty()) {
        DebuggerPrintf("RPCSystem::Register: id is empty. Ignoring.\n");
        return;
    };
    Net::RpcEntry rpc_entry;
    rpc_entry.id = id;
    rpc_entry.func_ptr = function;
    rpc_entry.serialize_ptr = RpcSerializeFunc<Ret, Args...>;
    rpc_entry.deserialize_ptr = RpcDeserializeFunc<Ret, Args...>;
    _rpcs.insert_or_assign(id, rpc_entry);
}

template<typename Ret, typename... Args>
void RPCSystem::Register(const char* id, Ret(*function)(Args...)) {
    Register(id ? std::string(id) : std::string(""), function);
}

template<typename Ret, typename... Args>
void RPCSystem::CallRPC(uint8_t source_id, uint8_t target_id, const std::string& id, Args... args) {
    auto rpc_iter = _rpcs.find(id);
    if(rpc_iter == _rpcs.end()) {
        return;
    }
    auto entry = rpc_iter->second;
    entry.source = source_id;
    entry.target = target_id;
    auto params = std::make_tuple<Args...>(std::forward<Args>(args)...);
    auto send_rpc = entry.serialize_ptr(entry, &params);
    send_rpc.sender = session.GetConnection(source_id);
    if(session.host_connection) {
        session.host_connection->Send(&send_rpc);
    }

    //SendRPC(session.my_connection->connection_index, target_id, id, args...);
}

template<typename Ret, typename... Args>
void RPCSystem::UnCallRPC(Net::Message& msg, RpcEntry& entry, Args... args) {
    auto params = std::make_tuple<Args...>(std::forward<Args>(args)...);
    (Ret)(*cb)(Args...) = reinterpret_cast<(Ret)(*)(Args...)>(entry.func_ptr);
    std::apply(cb, params);
}

} //End Net
