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
	XboxController() = default;
	~XboxController() = default;

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

	void SetRightMotorSpeedToMax();
	void SetLeftMotorSpeedToMax();

	void SetRightMotorSpeedAsPercentage(float speed);
	void SetLeftMotorSpeedAsPercentage(float speed);

	void SetRightMotorSpeed(unsigned short speed);
	void SetLeftMotorSpeed(unsigned short speed);

	void StopLeftMotor();
	void StopRightMotor();
	void StopMotors();

    float GetLeftTriggerPosition() const;
    float GetRightTriggerPosition() const;

    void BeginFrame();
    void EndFrame();
    void Render() const;

protected:
    void UpdateExternalButtonStateHelper();
	void SetMotorSpeed(int controllerNumber, bool isLeftMotor, unsigned short value);
private:
	KeyState m_xboxControllerButtons[GAMEPADCODE_MAX] = { false };
	Vector2 m_leftThumbDistance = {};
	Vector2 m_rightThumbDistance = {};
	Vector2 m_triggerDistance = {0.0f, 0.0f};
	unsigned short m_previousRawButtonState = 0;
	unsigned short m_currentRawButtonState = 0;
	unsigned short m_currentLeftMotorState = 0;
	unsigned short m_currentRightMotorState = 0;
    bool m_motorStateChanged = false;
	bool m_isConnected = false;
	bool m_isConnectedChanged = false;
};
