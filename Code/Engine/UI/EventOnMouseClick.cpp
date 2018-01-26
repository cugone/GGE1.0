#include "Engine/UI/EventOnMouseClick.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

	EventOnMouseClick::EventOnMouseClick()
    : Event(UI::EventType::MOUSECLICK)
{
    /* DO NOTHING */
}

void EventOnMouseClick::Trigger(EventData& event) {
    _callback(event);
}

void EventOnMouseClick::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}