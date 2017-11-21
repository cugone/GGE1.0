#pragma once

#include <functional>

#include "Engine/UI/Event.hpp"

namespace UI {

class Element;
struct EventData;

class EventOnEnter : public UI::Event {
public:
    EventOnEnter();
    virtual ~EventOnEnter() = default;

    virtual void Trigger(EventData& event) override;

    virtual void Subscribe(const std::function<void(EventData & event) >& callback) override;

protected:
private:
};

} //End UI