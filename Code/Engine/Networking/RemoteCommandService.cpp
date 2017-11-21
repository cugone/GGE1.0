#include "Engine/Networking/RemoteCommandService.hpp"

#include <regex>
#include <sstream>

#include "Engine/EngineConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/KerningFont.hpp"

#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/Connection.hpp"
#include "Engine/Networking/Address.hpp"
#include "Engine/Networking/TCPSocket.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

namespace Net {

RemoteCommandService* RemoteCommandService::s_instance = nullptr;

RemoteCommandService* RemoteCommandService::GetInstance() {
    return s_instance;
}

bool RemoteCommandService::ProcessSystemMessage(const SystemMessage& /*msg*/) {
    return false;
}

void RemoteCommandService::Initialize() {
    g_theConsole->RegisterCommand("rc",
    [&](const std::string& args) {

        //std::regex are expensive to construct but shouldn't be doing this more than once a frame anyway.
        //Matches:
        //Any string starting with at least one space, then a number,
        //then at least one occurrence of at least one space followed by at least one non-space
        std::regex args_idx_regex(R"(^[ ]+[0-9]+([ ]+[^ ]+)+)", std::regex::optimize);

        bool has_idx_specific = false;
        if(std::regex_match(args, args_idx_regex)) {
            has_idx_specific = true;
        }

        Arguments arg_set(args);
        std::size_t idx = (std::size_t)(-1);
        if(has_idx_specific) {
            arg_set.GetNext(idx);
        }
        std::string command_and_args;
        if(!arg_set.GetNext(command_and_args)) {
            g_theConsole->WarnMsg("No command specified.");
            return;
        }
        if(has_idx_specific) {
            this->SendCommandToOther(idx, command_and_args);
        } else {
            this->SendCommandToOthers(command_and_args);
        }
    },
    "Runs a remote command on all clients.");

    g_theConsole->RegisterCommand("rca",
    [&](const std::string& args) {
        Arguments arg_set(args);
        std::string command_and_args;
        if(!arg_set.GetNext(command_and_args)) {
            g_theConsole->WarnMsg("No command specified.");
            return;
        }
        this->SendCommandToAll(command_and_args);
    },
    "Runs a remote command all clients and the host.");


    g_theConsole->RegisterCommand("rcs_join",
                                  [&](const std::string& args) {
        Arguments arg_set(args);
        Net::Address addr;
        if(arg_set.GetNext(addr)) {
            this->Join(addr);
        }
    }, "Joins the RemoteCommandService at the specified address:port.");

    g_theConsole->RegisterCommand("rcs_restart",
                                  [&](const std::string& /*args*/) {
        session.Leave();
        if(!session.Join(Net::GetMyAddresses(RCS_PORT)[0])) {
            session.Host(RCS_PORT);
            session.StartListening();
        }
    }, "Restarts the RemoteCommandService session, hosting if possible.");

}

void RemoteCommandService::Update(float /*deltaSeconds*/) {
    if(session.IsRunning()) {
        session.Update();
    } else {
        if(!session.Join(Net::GetMyAddresses(RCS_PORT)[0])) {
            session.Host(RCS_PORT);
            session.StartListening();
        }
    }
}

void RemoteCommandService::Join(const Net::Address& addr) {
    session.Leave();
    session.Join(addr);
}

void RemoteCommandService::SetupMessageDefinitions() {
    session.RegisterMessageFunction(Net::MessageID::MESSAGE_ID_SEND_COMMAND, [this](Net::Message* msg) { this->OnCommand(msg); });
    session.RegisterMessageFunction(Net::MessageID::MESSAGE_ID_SEND_MESSAGE, [this](Net::Message* msg) { this->OnMessage(msg); });
}

void RemoteCommandService::OnCommand(Net::Message* msg) {
    std::string command_and_args;
    msg->readString(command_and_args);

    //TODO: Print someone sent me a command - addr: command_and_args

    current_sender = msg->sender;
    g_theConsole->OnMessagePrint.Subscribe_method(this, &RemoteCommandService::SendRcsMessage);
    g_theConsole->RunCommand(command_and_args);
    g_theConsole->OnMessagePrint.Unsubscribe_method(this, &RemoteCommandService::SendRcsMessage);
    current_sender = nullptr;
}

void RemoteCommandService::OnMessage(Net::Message* msg) {
    std::string response;
    msg->readString(response);
    std::ostringstream ss;
    ss << '[' << Net::NetAddressToString(msg->sender->address) << "] " << response;
    g_theConsole->NotifyMsg(ss.str());
}

void RemoteCommandService::SendRcsMessage(const std::string& str) {
    Net::Message msg(Net::MessageID::MESSAGE_ID_SEND_MESSAGE);
    msg.writeString(str);
    current_sender->Send(&msg);
}

void RemoteCommandService::SendCommandToOther(std::size_t idx, const std::string& command_and_args) {
    Net::Message message(Net::MessageID::MESSAGE_ID_SEND_COMMAND);
    message.writeString(command_and_args);
    session.SendMessageToOther(idx, message);
}

void RemoteCommandService::SendCommandToOthers(const std::string& command_and_args) {
    Net::Message message(Net::MessageID::MESSAGE_ID_SEND_COMMAND);
    message.writeString(command_and_args);
    session.SendMessageToOthers(message);
}

void RemoteCommandService::SendCommandToAll(const std::string& command_and_args) {
    Net::Message message(Net::MessageID::MESSAGE_ID_SEND_COMMAND);
    message.writeString(command_and_args);
    session.SendMessageToAll(message);
}

RemoteCommandService::RemoteCommandService(SimpleRenderer* renderer, std::size_t max_connections /*= 8*/) {
    s_instance = this;
    _renderer = renderer;
    SetupMessageDefinitions();
    session.max_connection_count = max_connections;
}

RemoteCommandService::~RemoteCommandService() {
    s_instance = nullptr;
    _renderer = nullptr;
}

void RemoteCommandService::Render() const {
    if(!g_theConsole->IsConsoleOpen()) {
        return;
    }

    auto font = g_theConsole->GetFont();
    auto dims = _renderer->_rhi_output->GetDimensions();
    float i = 1.0f;

    bool is_running = s_instance->session.IsRunning();
    bool is_ready = s_instance->session.IsReady();
    std::ostringstream ss;
    ss << "REMOTE COMMAND SERVICE [" << ((is_running) ? "RUNNING" : (is_ready ? "READY" : "STOPPED")) << "]";
    auto text_width = font->CalculateTextWidth(ss.str());
    _renderer->DrawTextLine(g_theConsole->GetFont(), ss.str() , Rgba::WHITE, dims.x - text_width, font->GetLineHeight() * i++);
    ss.str("");

    bool is_host = s_instance->session.AmIHost();
    bool is_client = s_instance->session.AmIClient();
    ss << "[" << (is_host ? "host" : (is_client ? "client" : "N/C")) << "] Join address: " << (s_instance->session.host_connection ? Net::NetAddressToString(s_instance->session.host_connection->address) : "NO HOST");
    text_width = font->CalculateTextWidth(ss.str());
    _renderer->DrawTextLine(g_theConsole->GetFont(), ss.str(), Rgba::WHITE, dims.x - text_width, font->GetLineHeight() * i++);
    ss.str("");

    ss << "client(s): " << s_instance->session.connection_list.size() << '/' << s_instance->session.max_connection_count;
    text_width = font->CalculateTextWidth(ss.str());
    _renderer->DrawTextLine(g_theConsole->GetFont(), ss.str(), Rgba::WHITE, dims.x - text_width, font->GetLineHeight() * i++);
    ss.str("");
    for(auto& conn : s_instance->session.connections) {
        if(conn == nullptr) {
            return;
        }
        ss << (conn->address == s_instance->session.host_connection->address ? " *" : " -") << " [" << conn->connection_index << "] " << Net::NetAddressToString(conn->address) << " [" << conn->owner->state << "] ";
        text_width = font->CalculateTextWidth(ss.str());
        _renderer->DrawTextLine(g_theConsole->GetFont(), ss.str(), Rgba::WHITE, dims.x - text_width, font->GetLineHeight() * i++);
        ss.str("");
    }

}

}