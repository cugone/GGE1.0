#include "Engine/UI/EventOnStartDrag.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

EventOnStartDrag::EventOnStartDrag()
: Event(UI::EventType::DRAGSTART)
{
    /* DO NOTHING */
}

void EventOnStartDrag::Trigger(EventData& event) {
    _callback(event);
}

void EventOnStartDrag::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}