#include "Engine/UI/Event.hpp"

namespace UI {

Event::Event(const EventType& eventType)
    : type(eventType)
{
    /* DO NOTHING */
}

Event::~Event() = default;

void Event::Trigger() {
    Trigger(data);
}

}