#pragma once

#include <cstdint>
#include <functional>
#include <map>

class Interval;

namespace Net {

class Connection;
class Message;
class Session;

namespace NetObjectSystem {

using net_object_id_t = uint16_t;

class NetObject;
class NetObjectTypeDefinition;

using append_create_info_cb = std::function<void(Net::Message* msg, void* local_obj)>;
using process_create_info_cb = std::function<void*(Net::Message* msg, Net::NetObjectSystem::NetObject* net_obj)>;

using append_destroy_info_cb = std::function<void(Net::Message* msg, void* local_obj)>;
using process_destroy_info_cb = std::function<void(Net::Message* msg, void* local_obj)>;

using create_snapshot_cb = std::function<void(Net::Message* msg, void* local_obj)>;
using destroy_snapshot_cb = std::function<void(Net::Message* msg, void* local_obj)>;

using get_snapshot_cb = std::function<void(void* current_snapshot, void* local_obj)>;
using send_snapshot_cb = std::function<void(Net::Message* msg, void* snapshot_obj)>;
using receive_snapshot_cb = std::function<void(void* snapshot_obj, Net::Message* msg)>;

using apply_snapshot_cb = std::function<void(void* local_obj, void* snapshot_obj)>;

extern uint16_t INVALID_NETWORK_ID;
extern std::map<net_object_id_t, NetObject*>* objects;
extern std::map<uint8_t, NetObjectTypeDefinition*>* definitions;
extern net_object_id_t last_used_id;
extern Net::Session* session;
extern Interval* interval;

class NetObject {
public:
    NetObject(NetObjectTypeDefinition* def);
    ~NetObject() = default;

    net_object_id_t GetNetId() const;
    void SetNetId(net_object_id_t id);

    NetObjectTypeDefinition* GetDefinition() const;

    void SetLocalObject(void* obj);
    void* GetLocalObject() const;

    void* last_recieved_snapshot = nullptr;
    void* current_snaphot = nullptr;

protected:
private:
    net_object_id_t _net_id;
    NetObjectTypeDefinition* _def;
    void* _local_object;
};

void Startup(Net::Session* game_session, float netTickHz = 20.0f);
void Shutdown();

uint16_t GetUnusedId();
NetObjectTypeDefinition* FindDefinition(uint8_t type_id);
NetObject* Replicate(void* object_ptr, uint8_t type_id);
void Register(NetObject* net_object);
void Unregister(NetObject* net_object);
NetObject* Find(NetObject* net_object);
NetObject* Find(net_object_id_t net_id);

void OnReceiveCreate(Net::Message* msg);
void StopReplication(net_object_id_t net_id);
void OnReceiveDestroy(Net::Message* msg);
void OnReceiveUpdate(Net::Message* msg);

void RegisterType(const uint8_t& type_id, const NetObjectTypeDefinition& def);

void SystemStep();
void SendNetObjectUpdates();
void SendNetObjectUpdateTo(Net::Connection* connection);

class NetObjectTypeDefinition {
public:
    uint8_t type_id;
    append_create_info_cb append_create_info;
    process_create_info_cb process_create_info;
    append_destroy_info_cb append_destroy_info;
    process_destroy_info_cb process_destroy_info;

    create_snapshot_cb create_snapshot;
    destroy_snapshot_cb destroy_snapshot;
    get_snapshot_cb get_snapshot;
    send_snapshot_cb send_snapshot;
    receive_snapshot_cb receive_snapshot;
    apply_snapshot_cb apply_snapshot;

protected:
private:
};

} //End NetObjectSystem

} //End Net