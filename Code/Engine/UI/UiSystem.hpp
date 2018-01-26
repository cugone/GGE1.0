#pragma once

#include <vector>

#include "Engine/Core/EngineSubsystem.hpp"

#include "Engine/Math/Vector2.hpp"

class SimpleRenderer;

namespace UI {

class Element;

class UiSystem : public EngineSubsystem {
public:

    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;

protected:
private:
    
};

} //End UI