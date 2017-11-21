#pragma once

#include <string>

#include "Engine/Math/Vector2.hpp"

namespace UI {

class Event;
enum class EventType;

struct Metric {
    Vector2 ratio;
    Vector2 unit;
};

enum class POSITION_MODE : unsigned char {
    UI_POSITION_ABSOLUTE,
    UI_POSITION_RELATIVE,
};

enum class FILL_MODE : unsigned char {
    FILL_MODE_FILL,
    FILL_MODE_FIT,
    FILL_MODE_STRETCH,
    FILL_MODE_TILE,
    FILL_MODE_CENTER,
    FILL_MODE_SPAN,
    FILL_MODE_CROP = FILL_MODE_FILL,
};

Event* CreateEventFromName(std::string name);
Event* CreateEventFromType(const EventType& type);

} //End UI