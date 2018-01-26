#include "Engine/Input/XboxController.hpp"

#include <limits>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; using 9_1_0 explicitly

#include "Engine/Math/MathUtils.hpp"

enum GamepadFlag {
	GAMEPADFLAG_DPAD_UP = 0x0001,
	GAMEPADFLAG_DPAD_DOWN = 0x0002,
	GAMEPADFLAG_DPAD_LEFT = 0x0004,
	GAMEPADFLAG_DPAD_RIGHT = 0x0008,
	GAMEPADFLAG_START = 0x0010,
	GAMEPADFLAG_BACK = 0x0020,
	GAMEPADFLAG_LEFT_THUMB = 0x0040,
	GAMEPADFLAG_RIGHT_THUMB = 0x0080,
	GAMEPADFLAG_LEFT_SHOULDER = 0x0100,
	GAMEPADFLAG_RIGHT_SHOULDER = 0x0200,
	GAMEPADFLAG_A = 0x1000,
	GAMEPADFLAG_B = 0x2000,
	GAMEPADFLAG_X = 0x4000,
	GAMEPADFLAG_Y = 0x8000,
};

void XboxController::UpdateExternalButtonStateHelper() {

	m_xboxControllerButtons[GAMEPADCODE_DPAD_UP].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_DPAD_UP) != 0;
	m_xboxControllerButtons[GAMEPADCODE_DPAD_DOWN].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_DPAD_DOWN) != 0;
	m_xboxControllerButtons[GAMEPADCODE_DPAD_LEFT].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_DPAD_LEFT) != 0;
	m_xboxControllerButtons[GAMEPADCODE_DPAD_RIGHT].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_DPAD_RIGHT) != 0;

	m_xboxControllerButtons[GAMEPADCODE_START].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_START) != 0;
	m_xboxControllerButtons[GAMEPADCODE_BACK].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_BACK) != 0;
	m_xboxControllerButtons[GAMEPADCODE_LEFT_THUMB].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_LEFT_THUMB) != 0;
	m_xboxControllerButtons[GAMEPADCODE_RIGHT_THUMB].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_RIGHT_THUMB) != 0;

	m_xboxControllerButtons[GAMEPADCODE_LEFT_SHOULDER].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_LEFT_SHOULDER) != 0;
	m_xboxControllerButtons[GAMEPADCODE_RIGHT_SHOULDER].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_RIGHT_SHOULDER) != 0;

	m_xboxControllerButtons[GAMEPADCODE_A].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_A) != 0;
	m_xboxControllerButtons[GAMEPADCODE_B].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_B) != 0;
	m_xboxControllerButtons[GAMEPADCODE_X].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_X) != 0;
	m_xboxControllerButtons[GAMEPADCODE_Y].m_isDown = (m_currentRawButtonState & GAMEPADFLAG_Y) != 0;

	m_xboxControllerButtons[GAMEPADCODE_DPAD_UP].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_DPAD_UP) ^ (m_previousRawButtonState & GAMEPADFLAG_DPAD_UP)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_DPAD_DOWN].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_DPAD_DOWN) ^ (m_previousRawButtonState & GAMEPADFLAG_DPAD_DOWN)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_DPAD_LEFT].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_DPAD_LEFT) ^ (m_previousRawButtonState & GAMEPADFLAG_DPAD_LEFT)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_DPAD_RIGHT].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_DPAD_RIGHT) ^ (m_previousRawButtonState & GAMEPADFLAG_DPAD_RIGHT)) != 0;

	m_xboxControllerButtons[GAMEPADCODE_START].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_START) ^ (m_previousRawButtonState & GAMEPADFLAG_START)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_BACK].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_BACK) ^ (m_previousRawButtonState & GAMEPADFLAG_BACK)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_LEFT_THUMB].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_LEFT_THUMB) ^ (m_previousRawButtonState & GAMEPADFLAG_LEFT_THUMB)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_RIGHT_THUMB].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_RIGHT_THUMB) ^ (m_previousRawButtonState & GAMEPADFLAG_RIGHT_THUMB)) != 0;

	m_xboxControllerButtons[GAMEPADCODE_LEFT_SHOULDER].m_justChanged = ((m_currentRawButtonState & GAMEPADCODE_LEFT_SHOULDER) ^ (m_previousRawButtonState & GAMEPADCODE_LEFT_SHOULDER)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_RIGHT_SHOULDER].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_RIGHT_SHOULDER) ^ (m_previousRawButtonState & GAMEPADFLAG_RIGHT_SHOULDER)) != 0;

	m_xboxControllerButtons[GAMEPADCODE_A].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_A) ^ (m_previousRawButtonState & GAMEPADFLAG_A)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_B].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_B) ^ (m_previousRawButtonState & GAMEPADFLAG_B)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_X].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_X) ^ (m_previousRawButtonState & GAMEPADFLAG_X)) != 0;
	m_xboxControllerButtons[GAMEPADCODE_Y].m_justChanged = ((m_currentRawButtonState & GAMEPADFLAG_Y) ^ (m_previousRawButtonState & GAMEPADFLAG_Y)) != 0;

}


void XboxController::SetRightMotorSpeedToMax() {
	SetRightMotorSpeedAsPercentage(1.0f);
}

void XboxController::SetLeftMotorSpeedToMax() {
	SetLeftMotorSpeedAsPercentage(1.0f);
}

void XboxController::SetLeftMotorSpeedAsPercentage(float speed) {
    SetLeftMotorSpeed(static_cast<unsigned short>(static_cast<float>((std::numeric_limits<unsigned short>::max)()) * speed));
}

void XboxController::SetRightMotorSpeedAsPercentage(float speed) {
    SetRightMotorSpeed(static_cast<unsigned short>(static_cast<float>((std::numeric_limits<unsigned short>::max)()) * speed));
}

void XboxController::SetRightMotorSpeed(unsigned short speed) {
    if(speed == m_currentRightMotorState) {
        return;
    }
	m_currentRightMotorState = speed;
    m_motorStateChanged = true;
}

void XboxController::SetLeftMotorSpeed(unsigned short speed) {
    if(speed == m_currentRightMotorState) {
        return;
    }
    m_motorStateChanged = true;
	m_currentLeftMotorState = speed;
}

void XboxController::StopLeftMotor() {
	SetLeftMotorSpeed(0);
}

void XboxController::StopRightMotor() {
	SetRightMotorSpeed(0);
}


void XboxController::StopMotors() {
	StopLeftMotor();
	StopRightMotor();
}

void XboxController::SetMotorSpeed(int controllerNumber, bool isLeftMotor, unsigned short value) {
	XINPUT_VIBRATION vibration;
	memset(&vibration, 0, sizeof(vibration));
	if (isLeftMotor) {
		vibration.wLeftMotorSpeed = value;
	}
	else {
		vibration.wRightMotorSpeed = value;
	}
	DWORD errorStatus = XInputSetState(controllerNumber, &vibration);
	if (errorStatus == ERROR_SUCCESS) {
		return;
	}
	else if (errorStatus == ERROR_DEVICE_NOT_CONNECTED) {
		if (m_isConnected) {
			m_isConnectedChanged = true;
			m_isConnected = false;
		}
		else {
			m_isConnectedChanged = false;
		}
		return;
	}
	else {
		return;
	}
}

<<<<<<< HEAD
void XboxController::Update(int controllerNumber) { // can be 0,1,2,3; API supports up to 4 Xbox controllers at once
	XINPUT_STATE xboxControllerState;
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(controllerNumber, &xboxControllerState);
=======
void XboxController::SetRightMotorSpeedToMax(int controllerNumber) {
    SetRightMotorSpeedAsPercentage(controllerNumber, 1.0f);
}

void XboxController::SetLeftMotorSpeedToMax(int controllerNumber) {
    SetLeftMotorSpeedAsPercentage(controllerNumber, 1.0f);
}

void XboxController::SetLeftMotorSpeedAsPercentage(int controllerNumber, float speed) {
    SetLeftMotorSpeed(controllerNumber, static_cast<unsigned short>(static_cast<float>((std::numeric_limits<unsigned short>::max)()) * speed));
}

void XboxController::SetRightMotorSpeedAsPercentage(int controllerNumber, float speed) {
    SetRightMotorSpeed(controllerNumber, static_cast<unsigned short>(static_cast<float>((std::numeric_limits<unsigned short>::max)()) * speed));
}

void XboxController::SetRightMotorSpeed(int controllerNumber, unsigned short speed) {
    SetMotorSpeed(controllerNumber, false, speed);
}

void XboxController::SetLeftMotorSpeed(int controllerNumber, unsigned short speed) {
    SetMotorSpeed(controllerNumber, true, speed);
}

void XboxController::StopLeftMotor(int controllerNumber) {
    SetLeftMotorSpeed(controllerNumber, 0);
}

void XboxController::StopRightMotor(int controllerNumber) {
    SetRightMotorSpeed(controllerNumber, 0);
}


void XboxController::StopMotors(int controllerNumber) {
    StopLeftMotor(controllerNumber);
    StopRightMotor(controllerNumber);
}

void XboxController::SetMotorSpeed(int controllerNumber, bool isLeftMotor, unsigned short value) {
    XINPUT_VIBRATION vibration;
    memset(&vibration, 0, sizeof(vibration));
    if (isLeftMotor) {
        vibration.wLeftMotorSpeed = value;
    }
    else {
        vibration.wRightMotorSpeed = value;
    }
    DWORD errorStatus = XInputSetState(controllerNumber, &vibration);
    if (errorStatus == ERROR_SUCCESS) {
        return;
    } else if (errorStatus == ERROR_DEVICE_NOT_CONNECTED) {
        if (m_isConnected) {
            m_isConnectedChanged = true;
            m_isConnected = false;
        }
        else {
            m_isConnectedChanged = false;
        }
        return;
    } else {
        return;
    }
}

void XboxController::Update(int controllerNumber) // can be 0,1,2,3; API supports up to 4 Xbox controllers at once
{
    XINPUT_STATE xboxControllerState;
    memset(&xboxControllerState, 0, sizeof(xboxControllerState));
    DWORD errorStatus = XInputGetState(controllerNumber, &xboxControllerState);

    if(errorStatus == ERROR_SUCCESS) {
        if(!m_isConnected) {
            m_isConnectedChanged = true;
            m_isConnected = true;
        } else {
            m_isConnectedChanged = false;
        }
        m_previousRawButtonState = m_currentRawButtonState;
        m_currentRawButtonState = xboxControllerState.Gamepad.wButtons;
>>>>>>> 250c99ae5abc6859bdd8dce1bf92f1e9bb25c01d

	if (errorStatus == ERROR_SUCCESS) {
		if (!m_isConnected) {
			m_isConnectedChanged = true;
			m_isConnected = true;
		}
		else {
			m_isConnectedChanged = false;
		}
		m_previousRawButtonState = m_currentRawButtonState;
		m_currentRawButtonState = xboxControllerState.Gamepad.wButtons;

		UpdateExternalButtonStateHelper();

		m_leftThumbDistance = Vector2(xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
		m_rightThumbDistance = Vector2(xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);
		m_triggerDistance = Vector2(xboxControllerState.Gamepad.bLeftTrigger, xboxControllerState.Gamepad.bRightTrigger);

		float leftRadius = m_leftThumbDistance.CalcLength();

		leftRadius = MathUtils::RangeMap<float>(leftRadius, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32000, 0.0f, 1.0f);
		leftRadius = MathUtils::Clamp<float>(leftRadius, 0.0f, 1.0f);

		m_leftThumbDistance.SetLength(leftRadius);

		float rightRadius = m_rightThumbDistance.CalcLength();

		rightRadius = MathUtils::RangeMap<float>(rightRadius, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32000, 0.0f, 1.0f);
		rightRadius = MathUtils::Clamp<float>(rightRadius, 0.0f, 1.0f);

		m_rightThumbDistance.SetLength(rightRadius);

		m_triggerDistance.x = MathUtils::RangeMap<float>(m_triggerDistance.x, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 255.0f, 0.0f, 1.0f);
		m_triggerDistance.y = MathUtils::RangeMap<float>(m_triggerDistance.y, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 255.0f, 0.0f, 1.0f);

        if(m_motorStateChanged) {
            m_motorStateChanged = false;
            SetMotorSpeed(controllerNumber, true, m_currentLeftMotorState);
            SetMotorSpeed(controllerNumber, false, m_currentRightMotorState);
        }

	}
	else if (errorStatus == ERROR_DEVICE_NOT_CONNECTED) {
		if (m_isConnected) {
			m_isConnectedChanged = true;
			m_isConnected = false;
		}
		else {
			m_isConnectedChanged = false;
		}
		return;
	}
	else {
		return;
	}
}

bool XboxController::IsButtonDown(const GamepadCode& code) const {
	return m_xboxControllerButtons[code].m_isDown;
}

bool XboxController::WasButtonJustPressed(const GamepadCode& code) const {
	return m_xboxControllerButtons[code].m_isDown && m_xboxControllerButtons[code].m_justChanged;
}

bool XboxController::IsButtonUp(const GamepadCode& code) const {
	return !m_xboxControllerButtons[code].m_isDown;
}

bool XboxController::WasButtonJustReleased(const GamepadCode& code) const {
	return !m_xboxControllerButtons[code].m_isDown && m_xboxControllerButtons[code].m_justChanged;
}

bool XboxController::IsButtonPressed(const GamepadCode& code) const {
	return m_xboxControllerButtons[code].m_isDown && !m_xboxControllerButtons[code].m_justChanged;
}

bool XboxController::IsDisconnected() const {
	return !m_isConnected && !m_isConnectedChanged;
}

bool XboxController::WasConnected() const {
	return m_isConnected && m_isConnectedChanged;
}

bool XboxController::IsConnected() const {
	return m_isConnected && !m_isConnectedChanged;
}

bool XboxController::WasDisconnected() const {
	return !m_isConnected && m_isConnectedChanged;
}

const Vector2& XboxController::GetLeftThumbPosition() const {
	return m_leftThumbDistance;
}

const Vector2& XboxController::GetRightThumbPosition() const {
	return m_rightThumbDistance;
}

float XboxController::GetLeftTriggerPosition() const {
	return m_triggerDistance.x;
}

float XboxController::GetRightTriggerPosition() const {
	return m_triggerDistance.y;
}

void XboxController::BeginFrame() {

}

void XboxController::EndFrame() {
	for (int buttonIndex = 0; buttonIndex < GAMEPADCODE_MAX; ++buttonIndex) {
		m_xboxControllerButtons[buttonIndex].m_justChanged = false;
	}
}

void XboxController::Render() const {

}