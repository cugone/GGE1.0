#include "Engine/UI/EventOnHover.hpp"

#include "Engine/UI/Element.hpp"

namespace UI {

EventOnHover::EventOnHover()
: Event(UI::EventType::HOVER)
{
    /* DO NOTHING */
}

void EventOnHover::Trigger(EventData& event) {
    _callback(event);
}

void EventOnHover::Subscribe(const std::function<void(EventData & event) >& callback) {
    _callback = callback;
}

}