#pragma once

#include <functional>
#include <vector>

namespace UI {

class Element;

enum class EventType {
    UNKNOWN,
    ENTER,
    LEAVE,
    HOVER,
    MOUSEDOWN,
    MOUSEUP,
	MOUSECLICK,
	MOUSEDBLCLICK,
    KEYDOWN,
    KEYUP,
	DRAGSTART,
	DRAGEND,
    MAX,
};

struct EventData {
    Element* source = nullptr;
    Element* target = nullptr;
    EventType type = EventType::UNKNOWN;
    unsigned char key = 0;
    unsigned char keycode = 0;
    unsigned char button = 0;
    bool consumeBubble = false;
    bool consumeCapture = false;
};

class Event {
public:
    Event() = default;
    Event(const EventType& eventType);
    virtual ~Event() = 0;

    EventType type = EventType::UNKNOWN;
    virtual void Trigger();
    virtual void Trigger(EventData& event) = 0;
    virtual void Subscribe(const std::function<void(EventData& event)>& callback) = 0;

    EventData data = EventData{};

protected:
    std::vector<Element*> _subscribers;
    std::function<void(EventData& data)> _callback = [](EventData&) {};
    
private:
};

} //End UI