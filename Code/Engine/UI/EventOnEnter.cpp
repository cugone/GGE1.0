#include "Engine/UI/EventOnEnter.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

EventOnEnter::EventOnEnter()
: Event(UI::EventType::ENTER)
{
    /* DO NOTHING */
}

void EventOnEnter::Trigger(EventData& event) {
    _callback(event);
}

void EventOnEnter::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}