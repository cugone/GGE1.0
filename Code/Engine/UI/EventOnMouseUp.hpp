#pragma once

#include <functional>

#include "Engine/UI/Event.hpp"

namespace UI {

class Element;
struct EventData;

class EventOnMouseUp : public UI::Event {
public:
	EventOnMouseUp();
    virtual ~EventOnMouseUp() = default;

    virtual void Trigger(EventData& event) override;

    virtual void Subscribe(const std::function<void(EventData & event) >& callback) override;

protected:
private:
};

} //End UI