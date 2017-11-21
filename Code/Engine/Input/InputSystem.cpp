#include "Engine/Input/InputSystem.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <algorithm>
#include <sstream>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Texture2D.hpp"

unsigned char ConvertKeyCodeToWinVK(const KeyCode& code);
KeyCode ConvertWinVKToKeyCode(unsigned char winVK);

void InputSystem::RegisterKeyDown(unsigned char keyIndex) {
    KeyState& keyState = m_keyStates[keyIndex];
    bool wasDown = keyState.m_isDown;
    bool isDown = true;
    bool justChanged = wasDown != isDown;

    keyState.m_isDown = isDown;
    keyState.m_justChanged = justChanged;
}

void InputSystem::RegisterKeyUp(unsigned char keyIndex) {
    KeyState& keyState = m_keyStates[keyIndex];
    bool wasDown = keyState.m_isDown;
    bool isDown = false;
    bool justChanged = wasDown != isDown;

    keyState.m_isDown = isDown;
    keyState.m_justChanged = justChanged;
}

bool InputSystem::IsKeyDown(const KeyCode& keyIndex) const {
    return m_keyStates[ConvertKeyCodeToWinVK(keyIndex)].m_isDown;
}

bool InputSystem::WasKeyJustPressed(const KeyCode& keyIndex) const {
    return IsKeyDown(keyIndex) && m_keyStates[ConvertKeyCodeToWinVK(keyIndex)].m_justChanged;
}

bool InputSystem::IsKeyUp(const KeyCode& keyIndex) const {
    return !m_keyStates[ConvertKeyCodeToWinVK(keyIndex)].m_isDown;
}

bool InputSystem::WasKeyJustReleased(const KeyCode& keyIndex) const {
    return IsKeyUp(keyIndex) && m_keyStates[ConvertKeyCodeToWinVK(keyIndex)].m_justChanged;
}

bool InputSystem::IsKeyPressed(const KeyCode& keyIndex) const {
    return IsKeyDown(keyIndex) && !m_keyStates[ConvertKeyCodeToWinVK(keyIndex)].m_justChanged;
}

bool InputSystem::WasMouseJustScrolledUp() const {
    return GetMouseWheelPositionNormalized() > 0;
}

bool InputSystem::WasMouseJustScrolledDown() const {
    return GetMouseWheelPositionNormalized() < 0;
}

void InputSystem::BeginFrame() {
    GetCursorScreenPosition();

    for(int i = 0; i < 4; ++i) {
        m_xboxControllers[i].Update(i);
    }
}

void InputSystem::EndFrame() {
    for(auto & m_keyState : m_keyStates) {
        m_keyState.m_justChanged = false;
    }
    m_mouseWheelPosition = 0;
}

void InputSystem::Render() const {
    //TODO: Draw debug overlay of keyboard and XBox controller states when F11 is pressed.
}

bool InputSystem::ProcessSystemMessage(const SystemMessage& msg) {
    unsigned char keyCode = (unsigned char)msg.wParam;
    switch(msg.wmMessageCode) {
        case WindowsSystemMessage::KEYBOARD_KEYDOWN: RegisterKeyDown(keyCode); return true;
        case WindowsSystemMessage::KEYBOARD_KEYUP: RegisterKeyUp(keyCode); return true;
        case WindowsSystemMessage::MOUSE_LBUTTONDOWN: RegisterKeyDown(keyCode); return true;
        case WindowsSystemMessage::MOUSE_LBUTTONUP: RegisterKeyUp(keyCode); return true;
        case WindowsSystemMessage::MOUSE_RBUTTONDOWN: RegisterKeyDown(keyCode); return true;
        case WindowsSystemMessage::MOUSE_RBUTTONUP: RegisterKeyUp(keyCode); return true;
        case WindowsSystemMessage::MOUSE_MBUTTONDOWN: RegisterKeyDown(keyCode); return true;
        case WindowsSystemMessage::MOUSE_MBUTTONUP: RegisterKeyUp(keyCode); return true;
        case WindowsSystemMessage::MOUSE_MOUSEWHEEL: UpdateMouseWheel(msg); return true;
        case WindowsSystemMessage::MOUSE_MOUSEMOVE: UpdateMousePosition(msg); return true;
    }
    return false;
}

const Vector2& InputSystem::GetMouseCoords() const {
    return m_mouseCoords;
}

int InputSystem::GetMouseWheelPosition() const {
    return m_mouseWheelPosition;
}

int InputSystem::GetMouseWheelPositionNormalized() const {
    if(m_mouseWheelPosition) {
        return m_mouseWheelPosition / std::abs(m_mouseWheelPosition);
    }
    return 0;
}

void InputSystem::SetMouseWheel(int wheelPosition) {
    m_mouseWheelPosition = wheelPosition;
}

unsigned char InputSystem::ConvertKeyCodeToScanCode(const KeyCode& code) {
    return ConvertKeyCodeToWinVK(code);
}

KeyCode InputSystem::ConvertScanCodeToKeyCode(unsigned char code) {
    return ConvertWinVKToKeyCode(code);
}

const Vector2& InputSystem::GetMouseDelta() const {
    return m_mouseDelta;
}

void InputSystem::UnlockMouseToCenter() {
    m_isMouseLocked = false;
}

void InputSystem::LockMouseToCenter() {
    m_isMouseLocked = true;
}

void InputSystem::UpdateMouseWheel(const SystemMessage& msg) {
    m_mouseWheelPosition = GET_WHEEL_DELTA_WPARAM(msg.wParam);
}

void InputSystem::UpdateMousePosition(const SystemMessage& msg) {
    m_mouseDelta = m_mouseCoords;
    POINTS p = MAKEPOINTS(msg.lParam);
    m_mouseCoords = Vector2(p.x, p.y);
    m_mouseDelta = m_mouseCoords - m_mouseDelta;
    SetMouseCursor(m_mouseCursor, m_mouseHotspot);
}

bool InputSystem::IsMouseLocked() {
    return m_isMouseLocked;
}

void InputSystem::ToggleLockMouseToCenter() {
    m_isMouseLocked = !m_isMouseLocked;
}

const XboxController& InputSystem::GetXboxController(int controllerIndex) const {
    return m_xboxControllers[controllerIndex];
}

void InputSystem::ShowMouseCursor(bool isNowVisible) {
    if(isNowVisible) {
        int currentShowCount = ShowCursor(TRUE);
        while(currentShowCount < 1) {
            currentShowCount = ShowCursor(TRUE);
        }
    } else {
        int currentShowCount = ShowCursor(FALSE);
        while(currentShowCount > 0) {
            currentShowCount = ShowCursor(FALSE);
        }
    }
}

void InputSystem::SetMouseCursor(Texture2D* img, const Vector2& hotspot /*= Vector2::ZERO*/) {
    if(img == nullptr) {
        ::SetCursor(::LoadCursorA(nullptr, IDC_ARROW));
        m_mouseHotspot = Vector2::ZERO;
        m_mouseArea = AABB2(Vector2{m_mouseCoords + m_mouseHotspot}, 0.0f, 0.0f);
        return;
    }
    m_mouseCursor = img;
    m_mouseHotspot = hotspot;
    auto dims = m_mouseCursor->GetDimensions();
    m_mouseArea = AABB2(Vector2{ m_mouseCoords + m_mouseHotspot }, static_cast<float>(dims.x), static_cast<float>(dims.y));
}

void InputSystem::SetCursorScreenPos(const IntVector2& screenPos) {
    SetCursorPos(screenPos.x, screenPos.y);
    m_mouseCoords = Vector2(screenPos);
    SetMouseCursor(m_mouseCursor, m_mouseHotspot);
}

IntVector2 InputSystem::GetCursorScreenPosition() const {
    POINT rawCursorPos;
    GetCursorPos(&rawCursorPos);
    return IntVector2(rawCursorPos.x, rawCursorPos.y);
}

IntVector2 InputSystem::GetScreenCenter() const {

    RECT desktopRect;
    HWND desktopWindowHandle = GetDesktopWindow();
    GetClientRect(desktopWindowHandle, &desktopRect);
    IntVector2 screenCenter((desktopRect.right + desktopRect.left) / 2, (desktopRect.bottom + desktopRect.top) / 2);
    return screenCenter;
}

KeyCode ConvertWinVKToKeyCode(unsigned char winVK) {
    switch (winVK) {
    case VK_LBUTTON: return KeyCode::LBUTTON;
    case VK_RBUTTON: return KeyCode::RBUTTON;
    case VK_CANCEL: return KeyCode::CANCEL;
    case VK_MBUTTON: return KeyCode::MBUTTON;
    case VK_BACK:  return KeyCode::BACK;
    case VK_TAB: return KeyCode::TAB;
    case VK_CLEAR: return KeyCode::CLEAR;
    case VK_RETURN: return KeyCode::RETURN;
    case VK_SHIFT: return KeyCode::SHIFT;
    case VK_CONTROL: return KeyCode::CTRL;
    case VK_MENU: return KeyCode::ALT;
    case VK_PAUSE: return KeyCode::PAUSE;
    case VK_CAPITAL: return KeyCode::CAPITAL;
    case VK_KANA: return KeyCode::KANA;
    //case VK_HANGUL: return KeyCode::HANGUL;
    case VK_JUNJA: return KeyCode::JUNJA;
    case VK_FINAL:  return KeyCode::FINAL;
    case VK_HANJA:  return KeyCode::HANJA;
    //case VK_KANJI:  return KeyCode::KANJI;
    case VK_ESCAPE: return KeyCode::ESCAPE;
    case VK_CONVERT:  return KeyCode::CONVERT;
    case VK_NONCONVERT:  return KeyCode::NONCONVERT;
    case VK_ACCEPT:  return KeyCode::ACCEPT;
    case VK_MODECHANGE:  return KeyCode::MODECHANGE;
    case VK_SPACE: return KeyCode::SPACE;
    case VK_PRIOR: return KeyCode::PRIOR;
    case VK_NEXT: return KeyCode::NEXT;
    case VK_END:  return KeyCode::END;
    case VK_HOME:  return KeyCode::HOME;
    case VK_LEFT:  return KeyCode::LEFT;
    case VK_UP:  return KeyCode::UP;
    case VK_RIGHT:  return KeyCode::RIGHT;
    case VK_DOWN:  return KeyCode::DOWN;
    case VK_SELECT:  return KeyCode::SELECT;
    case VK_PRINT:  return KeyCode::PRINT;
    case VK_EXECUTE:  return KeyCode::EXECUTE;
    case VK_SNAPSHOT: return KeyCode::SNAPSHOT;
    case VK_INSERT:  return KeyCode::INSERT;
    case VK_DELETE: return KeyCode::DELETE_KEYCODE;
    case VK_HELP:  return KeyCode::HELP;
    case '0':  return KeyCode::NUMERIC0;
    case '1':  return KeyCode::NUMERIC1;
    case '2':  return KeyCode::NUMERIC2;
    case '3':  return KeyCode::NUMERIC3;
    case '4':  return KeyCode::NUMERIC4;
    case '5':  return KeyCode::NUMERIC5;
    case '6':  return KeyCode::NUMERIC6;
    case '7':  return KeyCode::NUMERIC7;
    case '8':  return KeyCode::NUMERIC8;
    case '9':  return KeyCode::NUMERIC9;
    case 'A':  return KeyCode::A;
    case 'B':  return KeyCode::B;
    case 'C':  return KeyCode::C;
    case 'D':  return KeyCode::D;
    case 'E':  return KeyCode::E;
    case 'F':  return KeyCode::F;
    case 'G':  return KeyCode::G;
    case 'H':  return KeyCode::H;
    case 'I':  return KeyCode::I;
    case 'J':  return KeyCode::J;
    case 'K':  return KeyCode::K;
    case 'L':  return KeyCode::L;
    case 'M':  return KeyCode::M;
    case 'N':  return KeyCode::N;
    case 'O':  return KeyCode::O;
    case 'P':  return KeyCode::P;
    case 'Q':  return KeyCode::Q;
    case 'R':  return KeyCode::R;
    case 'S':  return KeyCode::S;
    case 'T':  return KeyCode::T;
    case 'U':  return KeyCode::U;
    case 'V':  return KeyCode::V;
    case 'W':  return KeyCode::W;
    case 'X':  return KeyCode::X;
    case 'Y':  return KeyCode::Y;
    case 'Z':  return KeyCode::Z;
    case VK_LWIN:  return KeyCode::LWIN;
    case VK_RWIN:  return KeyCode::RWIN;
    case VK_APPS:  return KeyCode::APPS;
    case VK_SLEEP:  return KeyCode::SLEEP;
    case VK_NUMPAD0:  return KeyCode::NUMPAD0;
    case VK_NUMPAD1:  return KeyCode::NUMPAD1;
    case VK_NUMPAD2:  return KeyCode::NUMPAD2;
    case VK_NUMPAD3:  return KeyCode::NUMPAD3;
    case VK_NUMPAD4:  return KeyCode::NUMPAD4;
    case VK_NUMPAD5:  return KeyCode::NUMPAD5;
    case VK_NUMPAD6:  return KeyCode::NUMPAD6;
    case VK_NUMPAD7:  return KeyCode::NUMPAD7;
    case VK_NUMPAD8:  return KeyCode::NUMPAD8;
    case VK_NUMPAD9:  return KeyCode::NUMPAD9;
    case VK_MULTIPLY:  return KeyCode::MULTIPLY;
    case VK_ADD:  return KeyCode::ADD;
    case VK_SEPARATOR:  return KeyCode::SEPARATOR;
    case VK_SUBTRACT:  return KeyCode::SUBTRACT;
    case VK_DECIMAL:  return KeyCode::DECIMAL;
    case VK_DIVIDE:  return KeyCode::DIVIDE;
    case VK_F1:  return KeyCode::F1;
    case VK_F2:  return KeyCode::F2;
    case VK_F3:   return KeyCode::F3;
    case VK_F4:   return KeyCode::F4;
    case VK_F5:   return KeyCode::F5;
    case VK_F6:   return KeyCode::F6;
    case VK_F7:   return KeyCode::F7;
    case VK_F8:   return KeyCode::F8;
    case VK_F9:   return KeyCode::F9;
    case VK_F10:   return KeyCode::F10;
    case VK_F11:   return KeyCode::F11;
    case VK_F12:   return KeyCode::F12;
    case VK_F13:   return KeyCode::F13;
    case VK_F14:   return KeyCode::F14;
    case VK_F15:   return KeyCode::F15;
    case VK_F16:   return KeyCode::F16;
    case VK_F17:   return KeyCode::F17;
    case VK_F18:   return KeyCode::F18;
    case VK_F19:   return KeyCode::F19;
    case VK_F20:   return KeyCode::F20;
    case VK_F21:   return KeyCode::F21;
    case VK_F22:   return KeyCode::F22;
    case VK_F23:   return KeyCode::F23;
    case VK_F24:   return KeyCode::F24;
    case VK_NUMLOCK: return KeyCode::NUMLOCK;
    case VK_SCROLL: return KeyCode::SCROLL;
    case VK_OEM_NEC_EQUAL:  return KeyCode::OEM_NEC_EQUAL;
    //case VK_OEM_FJ_JISHO:  return KeyCode::OEM_FJ_JISHO;
    case VK_OEM_FJ_MASSHOU:  return KeyCode::OEM_FJ_MASSHOU;
    case VK_OEM_FJ_TOUROKU:  return KeyCode::OEM_FJ_TOUROKU;
    case VK_OEM_FJ_LOYA:  return KeyCode::OEM_FJ_LOYA;
    case VK_OEM_FJ_ROYA:  return KeyCode::OEM_FJ_ROYA;
    case VK_LSHIFT:  return KeyCode::LSHIFT;
    case VK_RSHIFT:  return KeyCode::RSHIFT;
    case VK_LCONTROL: return KeyCode::LCONTROL;
    case VK_RCONTROL: return KeyCode::RCONTROL;
    case VK_LMENU: return KeyCode::LALT;
    case VK_RMENU: return KeyCode::RALT;
    case VK_BROWSER_BACK:  return KeyCode::BROWSER_BACK;
    case VK_BROWSER_FORWARD:  return KeyCode::BROWSER_FORWARD;
    case VK_BROWSER_REFRESH:  return KeyCode::BROWSER_REFRESH;
    case VK_BROWSER_STOP:  return KeyCode::BROWSER_STOP;
    case VK_BROWSER_SEARCH:  return KeyCode::BROWSER_SEARCH;
    case VK_BROWSER_FAVORITES:  return KeyCode::BROWSER_FAVORITES;
    case VK_BROWSER_HOME:  return KeyCode::BROWSER_HOME;
    case VK_VOLUME_MUTE:  return KeyCode::VOLUME_MUTE;
    case VK_VOLUME_DOWN:  return KeyCode::VOLUME_DOWN;
    case VK_VOLUME_UP:  return KeyCode::VOLUME_UP;
    case VK_MEDIA_NEXT_TRACK:  return KeyCode::MEDIA_NEXT_TRACK;
    case VK_MEDIA_PREV_TRACK:  return KeyCode::MEDIA_PREV_TRACK;
    case VK_MEDIA_STOP:  return KeyCode::MEDIA_STOP;
    case VK_MEDIA_PLAY_PAUSE:  return KeyCode::MEDIA_PLAY_PAUSE;
    case VK_LAUNCH_MAIL:  return KeyCode::LAUNCH_MAIL;
    case VK_LAUNCH_MEDIA_SELECT:  return KeyCode::LAUNCH_MEDIA_SELECT;
    case VK_LAUNCH_APP1:  return KeyCode::LAUNCH_APP1;
    case VK_LAUNCH_APP2:  return KeyCode::LAUNCH_APP2;
    case VK_OEM_1: return KeyCode::SEMICOLON;
    case VK_OEM_PLUS: return KeyCode::EQUALS;
    case VK_OEM_COMMA: return KeyCode::COMMA;
    case VK_OEM_MINUS: return KeyCode::MINUS;
    case VK_OEM_PERIOD: return KeyCode::PERIOD;
    case VK_OEM_2: return KeyCode::FORWARDSLASH;
    case VK_OEM_3: return KeyCode::TILDE;
    case VK_OEM_4: return KeyCode::LBRACKET;
    case VK_OEM_5: return KeyCode::BACKSLASH;
    case VK_OEM_6: return KeyCode::RBRACKET;
    case VK_OEM_7: return KeyCode::SINGLEQUOTE;
    case VK_OEM_8: return KeyCode::OEM_8;
    case VK_OEM_AX:  return KeyCode::OEM_AX;
    case VK_OEM_102: return KeyCode::OEM_102;
    case VK_ICO_HELP: return KeyCode::ICO_HELP;
    case VK_ICO_00: return KeyCode::ICO_00;
    case VK_PROCESSKEY:  return KeyCode::PROCESSKEY;
    case VK_ICO_CLEAR: return KeyCode::ICO_CLEAR;
    case VK_PACKET: return KeyCode::PACKET;
    case VK_OEM_RESET:  return KeyCode::OEM_RESET;
    case VK_OEM_JUMP:  return KeyCode::OEM_JUMP;
    case VK_OEM_PA1:  return KeyCode::OEM_PA1;
    case VK_OEM_PA2:  return KeyCode::OEM_PA2;
    case VK_OEM_PA3:  return KeyCode::OEM_PA3;
    case VK_OEM_WSCTRL:  return KeyCode::OEM_WSCTRL;
    case VK_OEM_CUSEL: return KeyCode::OEM_CUSEL;
    case VK_OEM_ATTN:  return KeyCode::OEM_ATTN;
    case VK_OEM_FINISH:  return KeyCode::OEM_FINISH;
    case VK_OEM_COPY:  return KeyCode::OEM_COPY;
    case VK_OEM_AUTO:  return KeyCode::OEM_AUTO;
    case VK_OEM_ENLW:  return KeyCode::OEM_ENLW;
    case VK_OEM_BACKTAB:  return KeyCode::OEM_BACKTAB;
    case VK_ATTN: return KeyCode::ATTN;
    case VK_CRSEL: return KeyCode::CRSEL;
    case VK_EXSEL: return KeyCode::EXSEL;
    case VK_EREOF: return KeyCode::EREOF;
    case VK_PLAY: return KeyCode::PLAY;
    case VK_ZOOM: return KeyCode::ZOOM;
    case VK_NONAME: return KeyCode::NONAME;
    case VK_PA1: return KeyCode::PA1;
    case VK_OEM_CLEAR: return KeyCode::OEM_CLEAR;
    default: return KeyCode::UNKNOWN;
    }
}

unsigned char ConvertKeyCodeToWinVK(const KeyCode& code) {
    switch (code) {
    case KeyCode::LBUTTON: return VK_LBUTTON;
    case KeyCode::RBUTTON: return VK_RBUTTON;
    case KeyCode::CANCEL: return VK_CANCEL;
    case KeyCode::MBUTTON: return VK_MBUTTON;
    case KeyCode::BACK: return VK_BACK; /* Also BACKSPACE */
    case KeyCode::TAB: return VK_TAB;
    case KeyCode::CLEAR: return VK_CLEAR;
    case KeyCode::RETURN: return VK_RETURN; /* Also ENTER */
    case KeyCode::SHIFT: return VK_SHIFT;
    case KeyCode::CTRL: return VK_CONTROL; /* Also CTRL */
    case KeyCode::MENU: return VK_MENU; /* Also ALT */
    case KeyCode::PAUSE: return VK_PAUSE;
    case KeyCode::CAPITAL: return VK_CAPITAL; /* Also CAPSLOCK */
    case KeyCode::KANA: return VK_KANA;
    case KeyCode::HANGUL: return VK_HANGUL; /* Also HANGEUL */
    case KeyCode::JUNJA: return VK_JUNJA;
    case KeyCode::FINAL: return VK_FINAL;
    case KeyCode::HANJA: return VK_HANJA;
    case KeyCode::KANJI: return VK_KANJI;
    case KeyCode::ESCAPE: return VK_ESCAPE; /* Also ESC */
    case KeyCode::CONVERT: return VK_CONVERT;
    case KeyCode::NONCONVERT: return VK_NONCONVERT;
    case KeyCode::ACCEPT: return VK_ACCEPT;
    case KeyCode::MODECHANGE: return VK_MODECHANGE;
    case KeyCode::SPACE: return VK_SPACE; /* Also SPACEBAR */
    case KeyCode::PRIOR: return VK_PRIOR; /* Also PAGEUP */
    case KeyCode::NEXT: return VK_NEXT; /* Also PAGEDOWN, PAGEDN */
    case KeyCode::END: return VK_END;
    case KeyCode::HOME: return VK_HOME;
    case KeyCode::LEFT: return VK_LEFT;
    case KeyCode::UP: return VK_UP;
    case KeyCode::RIGHT: return VK_RIGHT;
    case KeyCode::DOWN: return VK_DOWN;
    case KeyCode::SELECT: return VK_SELECT;
    case KeyCode::PRINT: return VK_PRINT;
    case KeyCode::EXECUTE: return VK_EXECUTE;
    case KeyCode::SNAPSHOT: return VK_SNAPSHOT; /* Also PRINTSCREEN */
    case KeyCode::INSERT: return VK_INSERT;
    case KeyCode::DELETE_KEYCODE: return VK_DELETE; /* I wanted to use DELETE here but I couldn't because FUCK MACROS */
    case KeyCode::HELP: return VK_HELP;
    case KeyCode::NUMERIC0: return '0';
    case KeyCode::NUMERIC1: return '1';
    case KeyCode::NUMERIC2: return '2';
    case KeyCode::NUMERIC3: return '3';
    case KeyCode::NUMERIC4: return '4';
    case KeyCode::NUMERIC5: return '5';
    case KeyCode::NUMERIC6: return '6';
    case KeyCode::NUMERIC7: return '7';
    case KeyCode::NUMERIC8: return '8';
    case KeyCode::NUMERIC9: return '9';
    case KeyCode::A: return 'A';
    case KeyCode::B: return 'B';
    case KeyCode::C: return 'C';
    case KeyCode::D: return 'D';
    case KeyCode::E: return 'E';
    case KeyCode::F: return 'F';
    case KeyCode::G: return 'G';
    case KeyCode::H: return 'H';
    case KeyCode::I: return 'I';
    case KeyCode::J: return 'J';
    case KeyCode::K: return 'K';
    case KeyCode::L: return 'L';
    case KeyCode::M: return 'M';
    case KeyCode::N: return 'N';
    case KeyCode::O: return 'O';
    case KeyCode::P: return 'P';
    case KeyCode::Q: return 'Q';
    case KeyCode::R: return 'R';
    case KeyCode::S: return 'S';
    case KeyCode::T: return 'T';
    case KeyCode::U: return 'U';
    case KeyCode::V: return 'V';
    case KeyCode::W: return 'W';
    case KeyCode::X: return 'X';
    case KeyCode::Y: return 'Y';
    case KeyCode::Z: return 'Z';
    case KeyCode::LWIN: return VK_LWIN;
    case KeyCode::RWIN: return VK_RWIN;
    case KeyCode::APPS: return VK_APPS;
    case KeyCode::SLEEP: return VK_SLEEP;
    case KeyCode::NUMPAD0: return VK_NUMPAD0;
    case KeyCode::NUMPAD1: return VK_NUMPAD1;
    case KeyCode::NUMPAD2: return VK_NUMPAD2;
    case KeyCode::NUMPAD3: return VK_NUMPAD3;
    case KeyCode::NUMPAD4: return VK_NUMPAD4;
    case KeyCode::NUMPAD5: return VK_NUMPAD5;
    case KeyCode::NUMPAD6: return VK_NUMPAD6;
    case KeyCode::NUMPAD7: return VK_NUMPAD7;
    case KeyCode::NUMPAD8: return VK_NUMPAD8;
    case KeyCode::NUMPAD9: return VK_NUMPAD9;
    case KeyCode::MULTIPLY: return VK_MULTIPLY;
    case KeyCode::ADD: return VK_ADD;
    case KeyCode::SEPARATOR: return VK_SEPARATOR; /* Also KEYPADENTER */
    case KeyCode::SUBTRACT: return VK_SUBTRACT;
    case KeyCode::DECIMAL: return VK_DECIMAL;
    case KeyCode::DIVIDE: return VK_DIVIDE;
    case KeyCode::F1:  return VK_F1;
    case KeyCode::F2:  return VK_F2;
    case KeyCode::F3:  return VK_F3;
    case KeyCode::F4:  return VK_F4;
    case KeyCode::F5:  return VK_F5;
    case KeyCode::F6:  return VK_F6;
    case KeyCode::F7:  return VK_F7;
    case KeyCode::F8:  return VK_F8;
    case KeyCode::F9:  return VK_F9;
    case KeyCode::F10: return VK_F10;
    case KeyCode::F11: return VK_F11;
    case KeyCode::F12: return VK_F12;
    case KeyCode::F13: return VK_F13;
    case KeyCode::F14: return VK_F14;
    case KeyCode::F15: return VK_F15;
    case KeyCode::F16: return VK_F16;
    case KeyCode::F17: return VK_F17;
    case KeyCode::F18: return VK_F18;
    case KeyCode::F19: return VK_F19;
    case KeyCode::F20: return VK_F20;
    case KeyCode::F21: return VK_F21;
    case KeyCode::F22: return VK_F22;
    case KeyCode::F23: return VK_F23;
    case KeyCode::F24: return VK_F24;
    case KeyCode::NUMLOCK: return VK_NUMLOCK;
    case KeyCode::SCROLL: return VK_SCROLL; /* Also SCROLLLOCK */
    case KeyCode::OEM_NEC_EQUAL: return VK_OEM_NEC_EQUAL;
    case KeyCode::OEM_FJ_JISHO: return VK_OEM_FJ_JISHO;
    case KeyCode::OEM_FJ_MASSHOU: return VK_OEM_FJ_MASSHOU;
    case KeyCode::OEM_FJ_TOUROKU: return VK_OEM_FJ_TOUROKU;
    case KeyCode::OEM_FJ_LOYA: return VK_OEM_FJ_LOYA;
    case KeyCode::OEM_FJ_ROYA: return VK_OEM_FJ_ROYA;
    case KeyCode::LSHIFT: return VK_LSHIFT;
    case KeyCode::RSHIFT: return VK_RSHIFT;
    case KeyCode::LCONTROL: return VK_LCONTROL; /* Also LCTRL */
    case KeyCode::RCONTROL: return VK_RCONTROL; /* Also RCTRL */
    case KeyCode::LMENU: return VK_LMENU; /* Also LALT*/
    case KeyCode::RMENU: return VK_RMENU; /* Also RALT */
    case KeyCode::BROWSER_BACK: return VK_BROWSER_BACK;
    case KeyCode::BROWSER_FORWARD: return VK_BROWSER_FORWARD;
    case KeyCode::BROWSER_REFRESH: return VK_BROWSER_REFRESH;
    case KeyCode::BROWSER_STOP: return VK_BROWSER_STOP;
    case KeyCode::BROWSER_SEARCH: return VK_BROWSER_SEARCH;
    case KeyCode::BROWSER_FAVORITES: return VK_BROWSER_FAVORITES;
    case KeyCode::BROWSER_HOME: return VK_BROWSER_HOME;
    case KeyCode::VOLUME_MUTE: return VK_VOLUME_MUTE;
    case KeyCode::VOLUME_DOWN: return VK_VOLUME_DOWN;
    case KeyCode::VOLUME_UP: return VK_VOLUME_UP;
    case KeyCode::MEDIA_NEXT_TRACK: return VK_MEDIA_NEXT_TRACK;
    case KeyCode::MEDIA_PREV_TRACK: return VK_MEDIA_PREV_TRACK;
    case KeyCode::MEDIA_STOP: return VK_MEDIA_STOP;
    case KeyCode::MEDIA_PLAY_PAUSE: return VK_MEDIA_PLAY_PAUSE;
    case KeyCode::LAUNCH_MAIL: return VK_LAUNCH_MAIL;
    case KeyCode::LAUNCH_MEDIA_SELECT: return VK_LAUNCH_MEDIA_SELECT;
    case KeyCode::LAUNCH_APP1: return VK_LAUNCH_APP1;
    case KeyCode::LAUNCH_APP2: return VK_LAUNCH_APP2;
    case KeyCode::OEM_1: return VK_OEM_1; /* ;: */
    case KeyCode::OEM_PLUS: return VK_OEM_PLUS; /* =+ */
    case KeyCode::OEM_COMMA: return VK_OEM_COMMA; /* ,< */
    case KeyCode::OEM_MINUS: return VK_OEM_MINUS; /* -_ */
    case KeyCode::OEM_PERIOD: return VK_OEM_PERIOD; /* .> */
    case KeyCode::OEM_2: return VK_OEM_2; /* /? */
    case KeyCode::OEM_3: return VK_OEM_3; /* `~ */
    case KeyCode::OEM_4: return VK_OEM_4; /* [{ */
    case KeyCode::OEM_5: return VK_OEM_5; /* \| */
    case KeyCode::OEM_6: return VK_OEM_6; /* ]} */
    case KeyCode::OEM_7: return VK_OEM_7; /* '" */
    case KeyCode::OEM_8: return VK_OEM_8; /* misc. unknown */
    case KeyCode::OEM_AX: return VK_OEM_AX;
    case KeyCode::OEM_102: return VK_OEM_102; /* RT 102's "<>" or "\|" */
    case KeyCode::ICO_HELP: return VK_ICO_HELP; /* Help key on ICO keyboard */
    case KeyCode::ICO_00: return VK_ICO_00; /* 00 key on ICO keyboard */
    case KeyCode::PROCESSKEY: return VK_PROCESSKEY;
    case KeyCode::ICO_CLEAR: return VK_ICO_CLEAR; /* Clear key on ICO keyboard */
    case KeyCode::PACKET: return VK_PACKET; /* Key is packet of data */
    case KeyCode::OEM_RESET: return VK_OEM_RESET;
    case KeyCode::OEM_JUMP: return VK_OEM_JUMP;
    case KeyCode::OEM_PA1: return VK_OEM_PA1;
    case KeyCode::OEM_PA2: return VK_OEM_PA2;
    case KeyCode::OEM_PA3: return VK_OEM_PA3;
    case KeyCode::OEM_WSCTRL: return VK_OEM_WSCTRL;
    case KeyCode::OEM_CUSEL: return VK_OEM_CUSEL;
    case KeyCode::OEM_ATTN: return VK_OEM_ATTN;
    case KeyCode::OEM_FINISH: return VK_OEM_FINISH;
    case KeyCode::OEM_COPY: return VK_OEM_COPY;
    case KeyCode::OEM_AUTO: return VK_OEM_AUTO;
    case KeyCode::OEM_ENLW: return VK_OEM_ENLW;
    case KeyCode::OEM_BACKTAB: return VK_OEM_BACKTAB;
    case KeyCode::ATTN: return VK_ATTN;
    case KeyCode::CRSEL: return VK_CRSEL;
    case KeyCode::EXSEL: return VK_EXSEL;
    case KeyCode::EREOF: return VK_EREOF;
    case KeyCode::PLAY: return VK_PLAY;
    case KeyCode::ZOOM: return VK_ZOOM;
    case KeyCode::NONAME: return VK_NONAME;
    case KeyCode::PA1: return VK_PA1;
    case KeyCode::OEM_CLEAR: return VK_OEM_CLEAR;
    case KeyCode::UNKNOWN: return 0xFF;
    default: return 0xFF;
    }
}


bool operator<(const KeyCode& lhs, const KeyCode& rhs) { return static_cast<unsigned short>(lhs) < static_cast<unsigned short>(rhs); }
bool operator<(unsigned char lhs, const KeyCode& rhs) { return lhs < static_cast<unsigned short>(rhs); }
bool operator<(const KeyCode& lhs, unsigned char rhs) { return static_cast<unsigned short>(lhs) < rhs; }
bool operator<(unsigned short lhs, const KeyCode& rhs) { return lhs < static_cast<unsigned short>(rhs); }
bool operator<(const KeyCode& lhs, unsigned short rhs) { return static_cast<unsigned short>(lhs) < rhs; }
bool operator<(unsigned int lhs, const KeyCode& rhs) { return lhs < static_cast<unsigned short>(rhs); }
bool operator<(const KeyCode& lhs, unsigned int rhs) { return static_cast<unsigned short>(lhs) < rhs; }
bool operator<(unsigned long lhs, const KeyCode& rhs) { return lhs < static_cast<unsigned short>(rhs); }
bool operator<(const KeyCode& lhs, unsigned long rhs) { return static_cast<unsigned short>(lhs) < rhs; }
bool operator<(unsigned long long lhs, const KeyCode& rhs) { return lhs < static_cast<unsigned short>(rhs); }
bool operator<(const KeyCode& lhs, unsigned long long rhs) { return static_cast<unsigned short>(lhs) < rhs; }

bool operator==(const KeyCode& lhs, const KeyCode& rhs) { return static_cast<unsigned short>(lhs) == static_cast<unsigned short>(rhs); }
bool operator==(unsigned char lhs, const KeyCode& rhs) { return lhs == static_cast<unsigned short>(rhs); }
bool operator==(const KeyCode& lhs, unsigned char rhs) { return static_cast<unsigned short>(lhs) == rhs; }
bool operator==(unsigned short lhs, const KeyCode& rhs) { return lhs == static_cast<unsigned short>(rhs); }
bool operator==(const KeyCode& lhs, unsigned short rhs) { return static_cast<unsigned short>(lhs) == rhs; }
bool operator==(unsigned int lhs, const KeyCode& rhs) { return lhs == static_cast<unsigned short>(rhs); }
bool operator==(const KeyCode& lhs, unsigned int rhs) { return static_cast<unsigned short>(lhs) == rhs; }
bool operator==(unsigned long lhs, const KeyCode& rhs) { return lhs == static_cast<unsigned short>(rhs); }
bool operator==(const KeyCode& lhs, unsigned long rhs) { return static_cast<unsigned short>(lhs) == rhs; }
bool operator==(unsigned long long lhs, const KeyCode& rhs) { return lhs == static_cast<unsigned short>(rhs); }
bool operator==(const KeyCode& lhs, unsigned long long rhs) { return static_cast<unsigned short>(lhs) == rhs; }

