#pragma once

#include <vector>

#include "Engine/Core/EngineSubsystem.hpp"

#include "Engine/Math/IntVector2.hpp"

class SimpleRenderer;

namespace UI {

class Element;

class UiSystem : public EngineSubsystem {
public:
    UiSystem(SimpleRenderer* renderer);
    ~UiSystem();

    void RegisterTopLevelElement(UI::Element* element);
    void UnregisterTopLevelElement(UI::Element* element);

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update(float deltaSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;

    IntVector2 GetMousePosition() const;

    SystemMessage GetLastMessage() const;

protected:
private:
    
    void UpdateMousePositionFromMsg(const SystemMessage &msg);

    std::vector<UI::Element*> _elements;
    SimpleRenderer* _renderer;
    IntVector2 _mousePos;
    SystemMessage _lastMessage;

};

} //End UI