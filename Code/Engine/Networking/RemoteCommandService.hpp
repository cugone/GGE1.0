#pragma once

#include <string>

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Networking/TCPSession.hpp"

class SimpleRenderer;

namespace Net {

class Message;
class Address;

constexpr unsigned short RCS_PORT = 0x379E;//14238;

class RemoteCommandService : public EngineSubsystem {
public:
    static RemoteCommandService* GetInstance();

    Net::TCPSession session;
    Net::Connection* current_sender;

    void Join(const Net::Address& addr);

    void SetupMessageDefinitions();

    void OnCommand(Net::Message* msg);
    void OnMessage(Net::Message* msg);

    void SendRcsMessage(const std::string& str);
    void SendCommandToOthers(const std::string& command_and_args);
    void SendCommandToAll(const std::string& command_and_args);

    virtual void Initialize() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;

    RemoteCommandService(SimpleRenderer* renderer, std::size_t max_connections = 8);
    virtual ~RemoteCommandService() override;

protected:
private:
    static RemoteCommandService* s_instance;
    SimpleRenderer* _renderer;
    
public:
    void SendCommandToOther(std::size_t idx, const std::string& command_and_args);
};

} //end Net