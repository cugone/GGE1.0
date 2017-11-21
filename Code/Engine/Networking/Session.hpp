#pragma once

#include <cstdint>
#include <limits>
#include <list>
#include <functional>
#include <vector>

namespace Net {

class Message;
enum class MessageID : unsigned char;
class MessageDefinition;
class Address;
class Connection;

enum class SessionState {
    DISCONNECTED
    ,CONNECTING
    ,JOINING
    ,READY
};

std::ostream& operator<<(std::ostream& out, const SessionState& state);
std::string SessionStateToString(const SessionState& state);

constexpr uint8_t INVALID_CONNECTION_INDEX = 0xff;


class Session {
public:
    Session();
    virtual ~Session();
    virtual bool Host(unsigned short port) = 0;
    virtual bool Join(const Net::Address& addr) = 0;
    virtual void Leave() = 0;
    virtual void Update() = 0;

    virtual bool StartListening() = 0;
    virtual void StopListening() = 0;
    virtual bool IsListening() = 0;

    void ProcessMessage(Net::Message* msg);

    bool RegisterMessageDefinition(const MessageID& messageId, Net::MessageDefinition& def);
    bool RegisterMessageFunction(const MessageID& messageId, const std::function<void(Net::Message*)>& callback);

    MessageDefinition* GetMessageDefinition(unsigned char id) const;

    inline bool AmIHost() const {
        return (my_connection == host_connection) && (host_connection != nullptr);
    }

    inline bool AmIClient() const {
        return (my_connection != host_connection) && (my_connection != nullptr);
    }

    inline bool IsRunning() const {
        return state != SessionState::DISCONNECTED;
    }

    inline bool IsReady() const {
        return state == SessionState::READY;
    }

    uint8_t GetFreeConnectionIndex() const;

    void JoinConnection(unsigned char idx, Net::Connection* conn);

    void DestroyConnection(Net::Connection* cp);

    Net::Connection* GetConnection(uint8_t idx);

    void SendMessageToOthersExceptSender(const Net::Message& msg);
    void SendMessageToOthers(const Net::Message& msg);
    void SendMessageToAll(const Net::Message& msg);
    void SendMessageToOther(std::size_t idx, Net::Message& msg);

    void SetState(const SessionState& newState);

    void SetMaxConnectionCount(std::size_t count);

    SessionState state;

    std::list<Net::Connection*> connection_list;
    
    std::vector<Net::Connection*> connections;
    std::size_t max_connection_count;

    Net::Connection* my_connection;
    Net::Connection* host_connection;

    std::vector<Net::MessageDefinition*> message_definitions;

protected:

private:

    bool AppendToConnectionList(Net::Connection* cp);
    bool RemoveFromConnectionList(Net::Connection* cp);
};

}