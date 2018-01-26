#pragma once

#include "Engine/Core/EngineSubsystem.hpp"

#include "Engine/Input/KeyState.hpp"
#include "Engine/Input/XboxController.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

class Texture2D;

enum class KeyCode : unsigned char {
    LBUTTON,
    RBUTTON,
    CANCEL,
    MBUTTON,
    BACK, /* Also BACKSPACE */
    BACKSPACE = BACK,
    TAB,
    CLEAR,
    RETURN, /* Also ENTER */
    ENTER = RETURN,
    SHIFT,
    CTRL, /* Also CTRL */
    MENU, /* Also ALT */
    ALT = MENU,
    PAUSE,
    CAPITAL, /* Also CAPSLOCK */
    CAPSLOCK = CAPITAL,
    KANA,
    HANGUL, /* Also HANGEUL */
    HANGEUL = HANGUL,
    JUNJA,
    FINAL,
    HANJA,
    KANJI,
    ESCAPE, /* Also ESC */
    ESC = ESCAPE,
    CONVERT,
    NONCONVERT,
    ACCEPT,
    MODECHANGE,
    SPACE, /* Also SPACEBAR */
    SPACEBAR = SPACE,
    PRIOR, /* Also PAGEUP */
    PAGEUP = PRIOR,
    NEXT, /* Also PAGEDOWN, PAGEDN */
    PAGEDOWN = NEXT,
    PAGEDN = NEXT,
    END,
    HOME,
    LEFT,
    UP,
    RIGHT,
    DOWN,
    SELECT,
    PRINT,
    EXECUTE,
    SNAPSHOT, /* Also PRINTSCREEN */
    PRINTSCREEN = SNAPSHOT,
    INSERT,
    DELETE_KEYCODE,
    HELP,
    NUMERIC0,
    NUMERIC1,
    NUMERIC2,
    NUMERIC3,
    NUMERIC4,
    NUMERIC5,
    NUMERIC6,
    NUMERIC7,
    NUMERIC8,
    NUMERIC9,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LWIN,
    RWIN,
    APPS,
    SLEEP,
    NUMPAD0,
    NUMPAD1,
    NUMPAD2,
    NUMPAD3,
    NUMPAD4,
    NUMPAD5,
    NUMPAD6,
    NUMPAD7,
    NUMPAD8,
    NUMPAD9,
    MULTIPLY,
    ADD,
    SEPARATOR, /* Also KEYPADENTER */
    NUMPADENTER = SEPARATOR,
    SUBTRACT,
    DECIMAL,
    DIVIDE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    NUMLOCK,
    SCROLL, /* Also SCROLLLOCK */
    SCROLLLOCK = SCROLL,
    OEM_NEC_EQUAL,
    OEM_FJ_JISHO,
    OEM_FJ_MASSHOU,
    OEM_FJ_TOUROKU,
    OEM_FJ_LOYA,
    OEM_FJ_ROYA,
    LSHIFT,
    RSHIFT,
    LCONTROL, /* Also LCTRL */
    LCTRL = LCONTROL,
    RCONTROL, /* Also RCTRL */
    RCTRL = RCONTROL,
    LMENU, /* Also LALT*/
    LALT = LMENU,
    RMENU, /* Also RALT */
    RALT = RMENU,
    BROWSER_BACK,
    BROWSER_FORWARD,
    BROWSER_REFRESH,
    BROWSER_STOP,
    BROWSER_SEARCH,
    BROWSER_FAVORITES,
    BROWSER_HOME,
    VOLUME_MUTE,
    VOLUME_DOWN,
    VOLUME_UP,
    MEDIA_NEXT_TRACK,
    MEDIA_PREV_TRACK,
    MEDIA_STOP,
    MEDIA_PLAY_PAUSE,
    LAUNCH_MAIL,
    LAUNCH_MEDIA_SELECT,
    LAUNCH_APP1,
    LAUNCH_APP2,
    OEM_1, /* ;: */
    SEMICOLON = OEM_1,
    OEM_PLUS, /* =+ */
    EQUALS = OEM_PLUS,
    OEM_COMMA, /* ,< */
    COMMA = OEM_COMMA,
    OEM_MINUS, /* -_ */
    MINUS = OEM_MINUS,
    OEM_PERIOD, /* .> */
    PERIOD = OEM_PERIOD,
    OEM_2, /* /? */
    FORWARDSLASH = OEM_2,
    FSLASH = OEM_2,
    OEM_3, /* `~ */
    BACKQUOTE = OEM_3,
    TILDE = OEM_3,
    OEM_4, /* [{ */
    LEFTBRACKET = OEM_4,
    LBRACKET = OEM_4,
    OEM_5, /* \| */
    BACKSLASH = OEM_5,
    OEM_6, /* ]} */
    RIGHTBRACKET = OEM_6,
    RBRACKET = OEM_6,
    OEM_7, /* '" */
    APOSTROPHE = OEM_7,
    SINGLEQUOTE = OEM_7,
    OEM_8, /* misc. unknown */
    OEM_AX,
    OEM_102, /* RT 102's "<>" or "\|" */
    ICO_HELP, /* Help key on ICO keyboard */
    ICO_00, /* 00 key on ICO keyboard */
    PROCESSKEY,
    ICO_CLEAR, /* Clear key on ICO keyboard */
    PACKET, /* Key is packet of data */
    OEM_RESET,
    OEM_JUMP,
    OEM_PA1,
    OEM_PA2,
    OEM_PA3,
    OEM_WSCTRL,
    OEM_CUSEL,
    OEM_ATTN,
    OEM_FINISH,
    OEM_COPY,
    OEM_AUTO,
    OEM_ENLW,
    OEM_BACKTAB,
    ATTN,
    CRSEL,
    EXSEL,
    EREOF,
    PLAY,
    ZOOM,
    NONAME,
    PA1,
    OEM_CLEAR,
    UNKNOWN = 0xFF,
};

bool operator<(const KeyCode& lhs, const KeyCode& rhs);
bool operator<(unsigned char lhs, const KeyCode& rhs);
bool operator<(const KeyCode& lhs, unsigned char rhs);
bool operator<(unsigned short lhs, const KeyCode& rhs);
bool operator<(const KeyCode& lhs, unsigned short rhs);
bool operator<(unsigned int lhs, const KeyCode& rhs);
bool operator<(const KeyCode& lhs, unsigned int rhs);
bool operator<(unsigned long lhs, const KeyCode& rhs);
bool operator<(const KeyCode& lhs, unsigned long rhs);
bool operator<(unsigned long long lhs, const KeyCode& rhs);
bool operator<(const KeyCode& lhs, unsigned long long rhs);

bool operator==(const KeyCode& lhs, const KeyCode& rhs);
bool operator==(unsigned char lhs, const KeyCode& rhs);
bool operator==(const KeyCode& lhs, unsigned char rhs);
bool operator==(unsigned short lhs, const KeyCode& rhs);
bool operator==(const KeyCode& lhs, unsigned short rhs);
bool operator==(unsigned int lhs, const KeyCode& rhs);
bool operator==(const KeyCode& lhs, unsigned int rhs);
bool operator==(unsigned long lhs, const KeyCode& rhs);
bool operator==(const KeyCode& lhs, unsigned long rhs);
bool operator==(unsigned long long lhs, const KeyCode& rhs);
bool operator==(const KeyCode& lhs, unsigned long long rhs);



class InputSystem : public EngineSubsystem {
public:
    InputSystem() = default;
    virtual ~InputSystem() override = default;

    //Updates (from Main_Win32)
    void RegisterKeyDown(unsigned char keyIndex);
    void RegisterKeyUp(unsigned char keyIndex);

    //Queries
    bool IsKeyDown(const KeyCode& key) const;
    bool WasKeyJustPressed(const KeyCode& key) const;

    bool IsKeyUp(const KeyCode& key) const;
    bool WasKeyJustReleased(const KeyCode& key) const;

    bool IsKeyPressed(const KeyCode& key) const;

    bool WasMouseJustScrolledUp() const;
    bool WasMouseJustScrolledDown() const;

	bool WasMouseLeftDoubleClicked() const;
	bool WasMouseMiddleDoubleClicked() const;
	bool WasMouseRightDoubleClicked() const;

    virtual void BeginFrame() override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const SystemMessage& msg) override;

    void SetCursorScreenPos(const IntVector2& screenPos);
    IntVector2 GetCursorScreenPosition() const;

    IntVector2 GetScreenCenter() const;

    const Vector2& GetMouseCoords() const;
    int GetMouseWheelPosition() const;
    int GetMouseWheelPositionNormalized() const;

    const XboxController& GetXboxController(int controllerIndex) const;
	XboxController& GetXboxController(int controllerIndex);

    void ShowMouseCursor(bool isNowVisible);
    void SetMouseCursor(Texture2D* img, const Vector2& hotspot = Vector2::ZERO);
    void SetMouseWheel(int wheelPosition);

    static unsigned char ConvertKeyCodeToScanCode(const KeyCode& code);
    static KeyCode ConvertScanCodeToKeyCode(unsigned char code);

    const Vector2& GetMouseDelta() const;

    void UnlockMouseToCenter();
    void LockMouseToCenter();
    bool IsMouseLocked();

    void ToggleLockMouseToCenter();

protected:
private:

	bool DidKeyChange(const KeyCode& keyIndex) const;
	bool DidKeyNotChange(const KeyCode& keyIndex) const;

	void SetMouseDoubleClickState(bool value);
	bool SetMouseMiddleDoubleClicked(bool value);
	bool SetMouseRightDoubleClicked(bool value);
	bool SetMouseLeftDoubleClicked(bool value);

    void UpdateMouseWheel(const SystemMessage& msg);
    void UpdateMousePosition(const SystemMessage& msg);

    static const unsigned short KEYCODE_MAX = 256u;
    KeyState m_keyStates[KEYCODE_MAX] = { false };
    XboxController m_xboxControllers[4] = {XboxController{}};
    Vector2 m_mouseCoords = Vector2::ZERO;
    Vector2 m_mouseDelta = Vector2::ZERO;
    Vector2 m_mouseHotspot = Vector2::ZERO;
    Texture2D* m_mouseCursor = nullptr;
    AABB2 m_mouseArea = AABB2{};
    int m_mouseWheelPosition = 0;
    bool m_isMouseLocked = false;
	bool m_wasMouseLeftDoubleClicked = false;
	bool m_wasMouseRightDoubleClicked = false;
	bool m_wasMouseMiddleDoubleClicked = false;
};