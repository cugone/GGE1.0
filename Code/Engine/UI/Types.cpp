#include "Engine/UI/Types.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/UI/Event.hpp"
#include "Engine/UI/EventOnEnter.hpp"

UI::Event* UI::CreateEventFromName(std::string name) {
    name = ToLowerCase(name);
    if(name == "onenter") {
        return new UI::EventOnEnter();
    } else if(name == "onleave") {
        return nullptr;
    } else if(name == "onmousedown") {
        return nullptr;
    }
    return nullptr;
}

UI::Event* UI::CreateEventFromType(const EventType& type) {
    switch(type) {
        case UI::EventType::ENTER:
            break;
        case UI::EventType::HOVER:
            break;
        case UI::EventType::KEYDOWN:
            break;
        case UI::EventType::KEYUP:
            break;
        case UI::EventType::LEAVE:
            break;
        case UI::EventType::MOUSEDOWN:
            break;
        case UI::EventType::MOUSEUP:
            break;
        case UI::EventType::UNKNOWN:
            ERROR_AND_DIE("Can not create event from base or unknwon type.");
        case UI::EventType::MAX:
            ERROR_AND_DIE("Attempting to create event from invalid type.");
        default:
            ERROR_AND_DIE("Unhandled event type at creation.");
    }
    return nullptr;
}
