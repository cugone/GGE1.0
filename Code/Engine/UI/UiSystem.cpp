#include "Engine/UI/UiSystem.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <algorithm>

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

#include "Engine/UI/Element.hpp"

UI::UiSystem::UiSystem(SimpleRenderer* renderer)
    : EngineSubsystem()
    , _renderer(renderer)
    , _elements{}
    , _mousePos{0,0}
    , _lastMessage{}
{
    /* DO NOTHING */
}

UI::UiSystem::~UiSystem() {
    for(auto& elem : _elements) {
        delete elem;
        elem = nullptr;
    }
    _elements.clear();
    _elements.shrink_to_fit();
}

bool UI::UiSystem::ProcessSystemMessage(const SystemMessage& msg) {
    //unsigned char asKey = (unsigned char)msg.wParam;
    //KeyCode asKeyCode = InputSystem::ConvertScanCodeToKeyCode(asKey);
    switch(msg.wmMessageCode) {
        case WindowsSystemMessage::MOUSE_MOUSEMOVE:
        {
            _lastMessage = msg;
            UpdateMousePositionFromMsg(msg);
            return false;
        }
        case WindowsSystemMessage::MOUSE_LBUTTONDBLCLK:
        {
            _lastMessage = msg;
            UpdateMousePositionFromMsg(msg);
            return false;
        }
        case WindowsSystemMessage::MOUSE_LBUTTONUP:
        {
            _lastMessage = msg;
            UpdateMousePositionFromMsg(msg);
            return false;
        }
        case WindowsSystemMessage::MOUSE_LBUTTONDOWN:
        {
            _lastMessage = msg;
            UpdateMousePositionFromMsg(msg);
            return false;
        }
    }
    return false;
}

IntVector2 UI::UiSystem::GetMousePosition() const {
    return _mousePos;
}

SystemMessage UI::UiSystem::GetLastMessage() const {
    return _lastMessage;
}

void UI::UiSystem::UpdateMousePositionFromMsg(const SystemMessage &msg) {
    int x = GET_X_LPARAM(msg.lParam);
    int y = GET_Y_LPARAM(msg.lParam);
    _mousePos.x = x;
    _mousePos.y = y;
}

void UI::UiSystem::Initialize() {
    /* DO NOTHING */
}

void UI::UiSystem::BeginFrame() {
    /* DO NOTHING */
}

void UI::UiSystem::Update(float deltaSeconds) {
    for(auto& elem : _elements) {
        elem->Update(deltaSeconds, _mousePos);
    }
}

void UI::UiSystem::Render() const {
#ifdef _DEBUG
    for(auto& elem : _elements) {
        elem->DebugRender(_renderer);
    }
#else
    for(auto& elem : _elements) {
        elem->Render(_renderer);
    }
#endif
}

void UI::UiSystem::EndFrame() {
    /* DO NOTHING */
}

void UI::UiSystem::RegisterTopLevelElement(UI::Element* element) {
    if(element && element->IsChild()) {
        return;
    }
    auto elem_iter = std::find(_elements.begin(), _elements.end(), element);
    if(elem_iter != _elements.end()) {
        return;
    }
    _elements.push_back(element);
}

void UI::UiSystem::UnregisterTopLevelElement(UI::Element* element) {
    if(element == nullptr) {
        return;
    }
    auto elem_iter = std::find(_elements.begin(), _elements.end(), element);
    if(elem_iter == _elements.end()) {
        return;
    }
    delete *elem_iter;
    *elem_iter = nullptr;
    auto index = std::distance(_elements.begin(), elem_iter);
    index;
    _elements.erase(elem_iter);
}
