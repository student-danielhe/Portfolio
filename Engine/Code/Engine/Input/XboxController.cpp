#include "Engine/Input/XboxController.hpp"
#include<Windows.h>
#include<Xinput.h>
#pragma comment( lib, "xinput" ) 
bool XboxController::IsConnected()const {
	return m_isConnected;
}
int XboxController::GetControllerID()const {
	return m_id;
}
AnalogJoystic const& XboxController::GetLeftStick()const {
	return m_leftStick;
}
AnalogJoystic const& XboxController::GetRightStick()const {
	return m_rightStick;
}
float XboxController::GetLeftTrigger()const {
	return m_leftTrigger;
}
float XboxController::GetRightTrigger()const {
	return m_rightTrigger;
}
KeyButtonState const& XboxController::GetButton(XboxButtonID bID)const {
	return m_buttons[bID];
}
bool XboxController::IsButtonDown(XboxButtonID bID)const {
	return m_buttons[bID].m_isPressed;
}
bool XboxController::WasButtonPressed(XboxButtonID bID)const{
	return m_buttons[bID].m_isPressed&&!m_buttons[bID].m_wasPressedLastFrame;
}
bool XboxController::WasButtonReleased(XboxButtonID bID)const {
	return m_buttons[bID].m_wasPressedLastFrame && !m_buttons[bID].m_isPressed;
}
void XboxController::Update() {
	XINPUT_STATE xboxControllerState;
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(m_id, &xboxControllerState);
	if (errorStatus != ERROR_SUCCESS) {
		Reset();
		m_isConnected = false;
		return;
	}
	m_isConnected = true;
	XINPUT_GAMEPAD const& state = xboxControllerState.Gamepad;
	
	UpdateJoyStick(m_leftStick,state.sThumbLX,state.sThumbLY);
	UpdateJoyStick(m_rightStick, state.sThumbRX, state.sThumbRY);

	UpdateTrigger(m_leftTrigger, state.bLeftTrigger);
	UpdateTrigger(m_rightTrigger, state.bRightTrigger);

	UpdateButton(XBOX_BUTTON_A, state.wButtons, XINPUT_GAMEPAD_A);
	UpdateButton(XBOX_BUTTON_B, state.wButtons, XINPUT_GAMEPAD_B);
	UpdateButton(XBOX_BUTTON_X, state.wButtons, XINPUT_GAMEPAD_X);
	UpdateButton(XBOX_BUTTON_Y, state.wButtons, XINPUT_GAMEPAD_Y);
	UpdateButton(XBOX_BUTTON_UP, state.wButtons, XINPUT_GAMEPAD_DPAD_UP);
	UpdateButton(XBOX_BUTTON_DOWN, state.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
	UpdateButton(XBOX_BUTTON_LEFT, state.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
	UpdateButton(XBOX_BUTTON_RIGHT, state.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	UpdateButton(XBOX_BUTTON_LSTICK, state.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
	UpdateButton(XBOX_BUTTON_RSTICK, state.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
	UpdateButton(XBOX_BUTTON_LB, state.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
	UpdateButton(XBOX_BUTTON_RB, state.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	UpdateButton(XBOX_BUTTON_START, state.wButtons, XINPUT_GAMEPAD_START);
	UpdateButton(XBOX_BUTTON_BACK, state.wButtons, XINPUT_GAMEPAD_BACK);
}
void XboxController::Reset() {
	m_id = -1;
	m_leftTrigger = 0.f;
	m_rightTrigger = 0.f;
	for (int i = 0; i < 14; i++) {
		m_buttons[i].m_isPressed = false;
		m_buttons[i].m_wasPressedLastFrame = false;
	}
	
	m_leftStick.Reset();
	m_rightStick.Reset();
}
void XboxController::UpdateJoyStick(AnalogJoystic& out_joystick, short rawX, short rawY) {
	out_joystick.UpdatePosition(rawX, rawY);
}
void XboxController::UpdateTrigger(float& out_trigger_value, unsigned char rawValue) {
	out_trigger_value = rawValue / 256.f;
}
void XboxController::UpdateButton(XboxButtonID bID, unsigned short buttonFlags, unsigned short buttonFlag) {
	KeyButtonState& button = m_buttons[bID];
	button.m_wasPressedLastFrame = button.m_isPressed;
	bool temp = ((buttonFlags & buttonFlag) == buttonFlag);
	button.m_isPressed = temp;
	
}