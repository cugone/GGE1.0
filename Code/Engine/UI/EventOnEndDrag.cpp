#include "Engine/UI/EventOnEndDrag.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

EventOnEndDrag::EventOnEndDrag()
: Event(UI::EventType::DRAGEND)
{
    /* DO NOTHING */
}

void EventOnEndDrag::Trigger(EventData& event) {
    _callback(event);
}

void EventOnEndDrag::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}