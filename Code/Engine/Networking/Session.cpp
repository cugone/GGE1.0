#include "Engine/Networking/Session.hpp"

#include <limits>
#include <type_traits>

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Networking/Connection.hpp"
#include "Engine/Networking/Message.hpp"

namespace Net {

std::ostream& operator<<(std::ostream& out, const SessionState& state) {
    out << SessionStateToString(state);
    return out;
}

std::string SessionStateToString(const SessionState& state) {
    switch(state) {
        case SessionState::DISCONNECTED:
            return std::string("DISCONNECTED");
        case SessionState::CONNECTING:
            return std::string("CONNECTING");
        case SessionState::JOINING:
            return std::string("JOINING");
        case SessionState::READY:
            return std::string("READY");
        default:
            return std::string("UNKNOWN");
    }
}

Session::Session()
    : state(SessionState::DISCONNECTED)
    , connection_list{}
    , connections{}
    , my_connection(nullptr)
    , host_connection(nullptr)
    , message_definitions((std::numeric_limits<std::underlying_type_t<MessageID>>::max)(), nullptr)
{
    /* DO NOTHING */
}

Session::~Session() {

    for(auto& connection : connections) {
        delete connection;
        connection = nullptr;
    }
    connections.clear();

    my_connection = nullptr;
    host_connection = nullptr;

    for(auto& definition : message_definitions) {
        delete definition;
        definition = nullptr;
    }
    message_definitions.clear();

}

void Session::ProcessMessage(Net::Message* msg) {
    if(msg == nullptr) {
        return;
    }
    Net::MessageDefinition* def = GetMessageDefinition(static_cast<std::underlying_type_t<Net::MessageID>>(msg->GetMessageType()));
    if(def) {
        def->handler(msg);
    }
}

uint8_t Session::GetFreeConnectionIndex() const {
    std::size_t index = 0;
    for(index = 0; index < connections.size(); ++index) {
        if(connections[index] == nullptr) {
            return static_cast<uint8_t>(index);
        }
    }

    if(index < max_connection_count) {
        return static_cast<uint8_t>(index);
    } else {
        return INVALID_CONNECTION_INDEX;
    }
}

void Session::JoinConnection(unsigned char idx, Net::Connection* conn) {
    if(conn == nullptr) {
        return;
    }
    conn->connection_index = idx;
    ASSERT_OR_DIE((idx >= connections.size() || connections[idx] == nullptr), "Session::JoinConnection: idx greater-equal to connection count.");
    if(idx >= connections.size()) {
        connections.resize(idx + 1);
    }
    connections[idx] = conn;
    AppendToConnectionList(conn);
}

bool Session::AppendToConnectionList(Net::Connection* cp) {
    if(cp == nullptr) {
        return false;
    }
    connection_list.emplace_back(cp);
    return true;
}

bool Session::RemoveFromConnectionList(Net::Connection* cp) {
    auto found_iter = std::find(connection_list.begin(), connection_list.end(), cp);
    if(found_iter != connection_list.end()) {
        connection_list.erase(std::remove(connection_list.begin(), connection_list.end(), cp), connection_list.end());
        return true;
    }
    return false;
}

bool Session::RegisterMessageDefinition(const MessageID& messageId, Net::MessageDefinition& def) {
    auto msgIdAsUtype = static_cast<std::underlying_type_t<MessageID>>(messageId);
    if(msgIdAsUtype > sizeof(std::underlying_type_t<MessageID>)) {
        return false;
    }
    Net::MessageDefinition* msg_def = new Net::MessageDefinition;
    msg_def->handler = def.handler;
    msg_def->type_index = messageId;
    message_definitions[msgIdAsUtype] = msg_def;
    return true;
}

bool Session::RegisterMessageFunction(const MessageID& messageId, const std::function<void(Net::Message*)>& callback) {
    auto msgIdAsUtype = static_cast<std::underlying_type_t<MessageID>>(messageId);
    int max = (std::numeric_limits<std::underlying_type_t<MessageID>>::max)();
    if(msgIdAsUtype > max) {
        return false;
    }
    Net::MessageDefinition* def = new Net::MessageDefinition;
    def->handler = callback;
    def->type_index = messageId;
    message_definitions[msgIdAsUtype] = def;
    return true;
}

MessageDefinition* Session::GetMessageDefinition(unsigned char id) const {
    if(id >= message_definitions.size()) {
        return nullptr;
    }
    return message_definitions[id];
}

void Session::DestroyConnection(Net::Connection* cp) {
    if(cp == nullptr) {
        return;
    }
    if(my_connection == cp) {
        my_connection = nullptr;
    }
    if(host_connection == cp) {
        host_connection = nullptr;
    }
    if(cp->connection_index != INVALID_CONNECTION_INDEX) {
        connections[cp->connection_index] = nullptr;
        cp->connection_index = INVALID_CONNECTION_INDEX;
    }
    cp->owner = nullptr;
    RemoveFromConnectionList(cp);
    delete cp;
    cp = nullptr;
}

Net::Connection* Session::GetConnection(uint8_t idx) {
    if(idx < connections.size()) {
        return connections[idx];
    }
    return nullptr;
}

void Session::SendMessageToOthersExceptSender(const Net::Message& msg) {
    for(auto cp : connection_list) {
        if(cp == my_connection || cp == msg.sender) {
            continue;
        }
        cp->Send(new Net::Message(msg));
    }
}

void Session::SendMessageToOthers(const Net::Message& msg) {
    for(auto cp : connection_list) {
        if(cp == my_connection) {
            continue;
        }
        cp->Send(new Net::Message(msg));
    }
}

void Session::SendMessageToOther(std::size_t idx, Net::Message& msg) {
    for(auto cp : connection_list) {
        if(cp->connection_index != idx) {
            continue;
        }
        cp->Send(new Net::Message(msg));
    }
}

void Session::SendMessageToAll(const Net::Message& msg) {
    for(auto cp : connection_list) {
        cp->Send(new Net::Message(msg));
    }
}

void Session::SetState(const SessionState& newState) {
    state = newState;
}

void Session::SetMaxConnectionCount(std::size_t count) {
    max_connection_count = count;
}

}