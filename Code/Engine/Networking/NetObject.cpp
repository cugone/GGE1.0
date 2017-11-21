#include "Engine/Networking/NetObject.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/Session.hpp"
#include "Engine/Networking/Interval.hpp"
#include "Engine/Networking/Connection.hpp"

namespace Net {

namespace NetObjectSystem {

uint16_t INVALID_NETWORK_ID = (uint16_t)-1;
std::map<net_object_id_t, NetObject*>* objects = nullptr;
std::map<uint8_t, NetObjectTypeDefinition*>* definitions = nullptr;
net_object_id_t last_used_id = 0;
Net::Session* session = nullptr;
Interval* interval = nullptr;

void Startup(Net::Session* game_session, float netTickHz /*= 20.0f*/) {
    session = game_session;
    session->RegisterMessageFunction(static_cast<Net::MessageID>(Net::MessageNetObjectID::MESSAGE_ID_NET_CREATE), [](Net::Message* msg) { OnReceiveCreate(msg); });
    session->RegisterMessageFunction(static_cast<Net::MessageID>(Net::MessageNetObjectID::MESSAGE_ID_NET_DESTROY), [](Net::Message* msg) { OnReceiveDestroy(msg); });
    session->RegisterMessageFunction(static_cast<Net::MessageID>(Net::MessageNetObjectID::MESSAGE_ID_NET_UPDATE), [](Net::Message* msg) { OnReceiveUpdate(msg); });

    objects = new std::map<net_object_id_t, NetObject*>();
    definitions = new std::map<uint8_t, NetObjectTypeDefinition*>();
    interval = new Interval;
    interval->set_frequency(netTickHz);
}

void Shutdown() {
    session = nullptr;
    if(objects) {
        for(auto& object : *objects) {
            delete object.second;
            object.second = nullptr;
        }
        objects->clear();
    }
    if(definitions) {
        for(auto& definition : *definitions) {
            delete definition.second;
            definition.second = nullptr;
        }
        definitions->clear();
    }
}

NetObject::NetObject(NetObjectTypeDefinition* def)
    : _def(def)
    , _net_id(INVALID_NETWORK_ID)
    , _local_object(nullptr)
{
    /* DO NOTHING */
}

net_object_id_t NetObject::GetNetId() const {
    return _net_id;
}

void NetObject::SetNetId(net_object_id_t id) {
    _net_id = id;
}

NetObjectTypeDefinition* NetObject::GetDefinition() const {
    return _def;
}

void NetObject::SetLocalObject(void* obj) {
    _local_object = obj;
}

void* NetObject::GetLocalObject() const {
    return _local_object;
}

net_object_id_t GetUnusedId() {
    while(objects->find(last_used_id) != objects->end()) {
        ++last_used_id;
    }
    return last_used_id;
}

NetObjectTypeDefinition* FindDefinition(uint8_t type_id) {
    auto found_iter = definitions->find(type_id);
    if(found_iter == definitions->end()) {
        return nullptr;
    }
    return found_iter->second;
}

NetObject* Replicate(void* object_ptr, uint8_t type_id) {
    NetObjectTypeDefinition* def = FindDefinition(type_id);
    if(def == nullptr) {
        return nullptr;
    }

    NetObject* nop = new NetObject(def);

    nop->SetLocalObject(object_ptr);
    nop->SetNetId(GetUnusedId());

    Register(nop);

    Net::Message create(static_cast<Net::MessageID>(Net::MessageNetObjectID::MESSAGE_ID_NET_CREATE));
    create.sender = session->my_connection;
    create.write(type_id);
    create.write(nop->GetNetId());

    def->append_create_info(&create, object_ptr);

    Net::Session* sp = session;
    sp->SendMessageToOthers(create);

    return nop;
}

void Register(NetObject* net_object) {
    objects->insert_or_assign(net_object->GetNetId(), net_object);
}

void Unregister(NetObject* net_object) {
    auto found_iter = objects->find(net_object->GetNetId());
    if(found_iter == objects->end()) {
        return;
    }
    auto id = net_object->GetNetId();
    objects->erase(id);
}

NetObject* Find(NetObject* net_object) {
    auto found_iter = objects->find(net_object->GetNetId());
    if(found_iter == objects->end()) {
        return nullptr;
    }
    return found_iter->second;
}

NetObject* Find(net_object_id_t net_id) {
    auto found_iter = objects->find(net_id);
    if(found_iter == objects->end()) {
        return nullptr;
    }
    return found_iter->second;
}

void OnReceiveCreate(Net::Message* msg) {

    uint8_t type_id;
    msg->read(type_id);
    net_object_id_t net_id;
    msg->read(net_id);

    NetObjectTypeDefinition* def = FindDefinition(type_id);
    ASSERT_OR_DIE(def != nullptr, "Type definition not registered.");

    NetObject* nop = new NetObject(def);
    nop->SetNetId(net_id);

    void* local_object = def->process_create_info(msg, nop);
    nop->SetLocalObject(local_object);

    Register(nop);

}

void StopReplication(net_object_id_t net_id) {
    NetObject* nop = Find(net_id);
    if(nop == nullptr) {
        return;
    }

    Unregister(nop);

    Net::Message msg(static_cast<Net::MessageID>(MessageNetObjectID::MESSAGE_ID_NET_DESTROY));
    msg.sender = session->my_connection;
    msg.write(nop->GetNetId());

    if(nop->GetDefinition()->append_destroy_info) {
        nop->GetDefinition()->append_destroy_info(&msg, nop->GetLocalObject());
    }

    session->SendMessageToOthers(msg);

    delete nop;
    nop = nullptr;

}

void OnReceiveDestroy(Net::Message* msg) {
    
    uint16_t net_id;
    msg->read(net_id);
    NetObject* nop = Find(net_id);

    if(nop == nullptr) {
        return;
    }

    Unregister(nop);

    nop->GetDefinition()->process_destroy_info(msg, nop->GetLocalObject());

    delete nop;
    nop = nullptr;
}

void OnReceiveUpdate(Net::Message* msg) {
    uint16_t net_id;
    msg->read(net_id);

    NetObject* nop = Find(net_id);
    if(nop == nullptr) {
        return;
    }

    nop->GetDefinition()->receive_snapshot(nop->last_recieved_snapshot, msg);

}

void RegisterType(const uint8_t& type_id, const NetObjectTypeDefinition& def) {
    auto* def_copy = new NetObjectTypeDefinition();
    def_copy->type_id = def.type_id;
    def_copy->append_create_info = def.append_create_info;
    def_copy->process_create_info = def.process_create_info;
    def_copy->append_destroy_info = def.append_destroy_info;
    def_copy->process_destroy_info = def.process_destroy_info;
    def_copy->apply_snapshot = def.apply_snapshot;
    def_copy->create_snapshot = def.create_snapshot;
    def_copy->destroy_snapshot = def.destroy_snapshot;
    def_copy->get_snapshot = def.get_snapshot;
    def_copy->receive_snapshot = def.receive_snapshot;
    def_copy->send_snapshot = def.send_snapshot;
    definitions->insert(std::make_pair(type_id, def_copy));
}


void SystemStep() {
    if(interval->check_and_reset()) {
        if(session->AmIHost()) {
            SendNetObjectUpdates();
        }
    }
    if(session->AmIHost()) {
        return;
    }

    for(auto& obj : *objects) {
        NetObject* nop = obj.second;
        if(nop == nullptr) {
            continue;
        }
        nop->GetDefinition()->apply_snapshot(nop->GetLocalObject(), nop->last_recieved_snapshot);
    }

}

void SendNetObjectUpdates() {

    for(auto& obj : *objects) {
        NetObject* nop = obj.second;
        if(nop == nullptr) {
            continue;
        }
        nop->GetDefinition()->get_snapshot(nop->current_snaphot, nop->GetLocalObject());
    }

    for(auto cur_connection : session->connections) {
        if(cur_connection != session->my_connection) {
            SendNetObjectUpdateTo(cur_connection);
        }
    }

}

void SendNetObjectUpdateTo(Net::Connection* connection) {
    for(auto obj : *objects) {
        NetObject* nop = obj.second;
        if(nop == nullptr) {
            continue;
        }
        Net::Message update_msg = Net::Message(static_cast<Net::MessageID>(Net::MessageNetObjectID::MESSAGE_ID_NET_UPDATE));
        update_msg.write(nop->GetNetId());
        nop->GetDefinition()->send_snapshot(&update_msg, nop->current_snaphot);
        if(connection) {
            connection->Send(&update_msg);
        }
    }
}

} //End NetObjectSystem

} //End Net