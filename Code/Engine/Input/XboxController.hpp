#pragma once

#include "Engine/Input/KeyState.hpp"
#include "Engine/Math/Vector2.hpp"

enum GamepadCode {
    GAMEPADCODE_DPAD_UP = 0,
    GAMEPADCODE_DPAD_DOWN,
    GAMEPADCODE_DPAD_LEFT,
    GAMEPADCODE_DPAD_RIGHT,
    GAMEPADCODE_START,
    GAMEPADCODE_BACK,
    GAMEPADCODE_LEFT_THUMB,
    GAMEPADCODE_RIGHT_THUMB,
    GAMEPADCODE_LEFT_SHOULDER,
    GAMEPADCODE_RIGHT_SHOULDER,
    GAMEPADCODE_A,
    GAMEPADCODE_B,
    GAMEPADCODE_X,
    GAMEPADCODE_Y,
    GAMEPADCODE_MAX,
};


class XboxController {

public:
	XboxController();
	~XboxController();

    void Update(int controllerNumber);

    bool IsButtonDown(const GamepadCode& code) const;
    bool WasButtonJustPressed(const GamepadCode& code) const;

    bool IsButtonUp(const GamepadCode& code) const;
    bool WasButtonJustReleased(const GamepadCode& code) const;

    bool IsButtonPressed(const GamepadCode& code) const;

    bool IsDisconnected() const;
    bool WasConnected() const;
    bool IsConnected() const;
    bool WasDisconnected() const;

    const Vector2& GetLeftThumbPosition() const;
    const Vector2& GetRightThumbPosition() const;

    float GetLeftTriggerPosition() const;
    float GetRightTriggerPosition() const;

    void BeginFrame();
    void EndFrame();
    void Render() const;

protected:
    void UpdateExternalButtonStateHelper();
private:
    KeyState m_xboxControllerButtons[GAMEPADCODE_MAX];
    Vector2 m_leftThumbDistance;
    Vector2 m_rightThumbDistance;
    Vector2 m_triggerDistance;
    unsigned short m_previousRawButtonState;
    unsigned short m_currentRawButtonState;
    bool m_isConnected;
    bool m_isConnectedChanged;
};