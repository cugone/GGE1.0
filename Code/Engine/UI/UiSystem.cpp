#include "Engine/UI/UiSystem.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <algorithm>

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"

#include "Engine/UI/Element.hpp"

bool UI::UiSystem::ProcessSystemMessage(const SystemMessage& /*msg*/) {
    return false;
}
