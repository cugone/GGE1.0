#pragma once

#include <functional>

#include "Engine/UI/Event.hpp"

namespace UI {

class Element;
struct EventData;

class EventOnLeave : public UI::Event {
public:
    EventOnLeave();
    virtual ~EventOnLeave() = default;

    virtual void Trigger(EventData& event) override;

    virtual void Subscribe(const std::function<void(EventData & event) >& callback) override;

protected:
private:
};

} //End UI