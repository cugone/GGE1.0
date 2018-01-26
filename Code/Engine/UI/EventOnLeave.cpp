#include "Engine/UI/EventOnLeave.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

EventOnLeave::EventOnLeave()
    : Event(UI::EventType::LEAVE)
{
    /* DO NOTHING */
}

void EventOnLeave::Trigger(EventData& event) {
    _callback(event);
}

void EventOnLeave::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}