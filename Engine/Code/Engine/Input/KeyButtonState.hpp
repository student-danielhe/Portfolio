#pragma once
class KeyButtonState {
public:
	KeyButtonState(){}
	~KeyButtonState(){}
	bool m_isPressed=false;
	bool m_wasPressedLastFrame = false;
};