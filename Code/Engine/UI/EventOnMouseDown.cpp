#include "Engine/UI/EventOnMouseDown.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

EventOnMouseDown::EventOnMouseDown()
    : Event(UI::EventType::MOUSEDOWN)
{
    /* DO NOTHING */
}

void EventOnMouseDown::Trigger(EventData& event) {
    _callback(event);
}

void EventOnMouseDown::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}