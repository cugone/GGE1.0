#pragma once

#include <functional>

#include "Engine/UI/Event.hpp"

namespace UI {

class Element;
struct EventData;

class EventOnMouseClick : public UI::Event {
public:
	EventOnMouseClick();
    virtual ~EventOnMouseClick() = default;

    virtual void Trigger(EventData& event) override;

    virtual void Subscribe(const std::function<void(EventData & event) >& callback) override;

protected:
private:
};

} //End UI