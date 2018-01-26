#include "Engine/UI/EventOnMouseDblClick.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

	EventOnMouseDblClick::EventOnMouseDblClick()
    : Event(UI::EventType::MOUSEDBLCLICK)
{
    /* DO NOTHING */
}

void EventOnMouseDblClick::Trigger(EventData& event) {
    _callback(event);
}

void EventOnMouseDblClick::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}