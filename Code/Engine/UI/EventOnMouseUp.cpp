#include "Engine/UI/EventOnMouseUp.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

	EventOnMouseUp::EventOnMouseUp()
    : Event(UI::EventType::MOUSEUP)
{
    /* DO NOTHING */
}

void EventOnMouseUp::Trigger(EventData& event) {
    _callback(event);
}

void EventOnMouseUp::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}