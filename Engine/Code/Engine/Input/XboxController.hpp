#pragma once
#include "KeyButtonState.hpp"
#include "AnalogJoystic.hpp"
enum XboxButtonID {
	XBOX_BUTTON_A,
	XBOX_BUTTON_B,
	XBOX_BUTTON_Y,
	XBOX_BUTTON_X,
	XBOX_BUTTON_UP,
	XBOX_BUTTON_DOWN,
	XBOX_BUTTON_LEFT,
	XBOX_BUTTON_RIGHT,
	XBOX_BUTTON_RSTICK,
	XBOX_BUTTON_LSTICK,
	XBOX_BUTTON_RB,
	XBOX_BUTTON_LB,
	XBOX_BUTTON_START,
	XBOX_BUTTON_BACK,
};
class XboxController {

	friend class InputSystem;

public:
	XboxController(){}
	~XboxController(){}
	
	bool IsConnected()const;
	int GetControllerID()const;
	AnalogJoystic const& GetLeftStick()const;
	AnalogJoystic const& GetRightStick()const;
	float GetLeftTrigger()const;
	float GetRightTrigger()const;
	KeyButtonState const& GetButton(XboxButtonID bID)const;
	bool IsButtonDown(XboxButtonID bID)const;
	bool WasButtonPressed(XboxButtonID bID)const;
	bool WasButtonReleased(XboxButtonID bID)const;
private:
	void Update();
	void Reset();
	void UpdateJoyStick(AnalogJoystic& out_joystick, short rawX, short rawY);
	void UpdateTrigger(float& out_trigger_value, unsigned char rawValue);
	void UpdateButton(XboxButtonID bID, unsigned short buttonFlags, unsigned short buttonFlag);

	int m_id = -1;
	bool m_isConnected = false;
	float m_leftTrigger = 0.f;
	float m_rightTrigger = 0.f;
	KeyButtonState m_buttons[14];
	AnalogJoystic m_leftStick;
	AnalogJoystic m_rightStick;

};