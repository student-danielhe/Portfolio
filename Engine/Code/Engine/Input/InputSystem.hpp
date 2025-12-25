#pragma once
#include"XboxController.hpp"


//------------
extern unsigned char const KEYCODE_F1;
extern unsigned char const KEYCODE_F2;
extern unsigned char const KEYCODE_F3;
extern unsigned char const KEYCODE_F4;
extern unsigned char const KEYCODE_F5;
extern unsigned char const KEYCODE_F6;
extern unsigned char const KEYCODE_F7;
extern unsigned char const KEYCODE_F8;
extern unsigned char const KEYCODE_F9;
extern unsigned char const KEYCODE_F10;
extern unsigned char const KEYCODE_F11;
extern unsigned char const KEYCODE_F12;
extern unsigned char const KEYCODE_UP;
extern unsigned char const KEYCODE_DOWN;
extern unsigned char const KEYCODE_LEFT;
extern unsigned char const KEYCODE_RIGHT;
extern unsigned char const KEYCODE_DEV_CONSOLE;
extern unsigned char const KEYCODE_LEFT_MOUSE;
extern unsigned char const KEYCODE_RIGHT_MOUSE;
extern unsigned char const KEYCODE_TILDE;
extern unsigned char const KEYCODE_LEFTBRACKET;
extern unsigned char const KEYCODE_RIGHTBRACKET;
extern unsigned char const KEYCODE_BACKSPACE;
extern unsigned char const KEYCODE_INSERT;
extern unsigned char const KEYCODE_DELETE;
extern unsigned char const KEYCODE_HOME;
extern unsigned char const KEYCODE_END;
extern unsigned char const KEYCODE_ENTER;
extern unsigned char const KEYCODE_ESCAPE;
extern unsigned char const KEYCODE_SHIFT;
extern unsigned char const KEYCODE_EQUAL;
extern unsigned char const KEYCODE_CONTROL;
extern unsigned char const KEYCODE_DECIMAL;
//--------
#include "Engine/Core/NamedStrings.hpp"

typedef NamedStrings EventArgs;

struct InputSystemConfig {

};
enum class CursorMode
{
	POINTER,
	FPS,
};

struct CursorState
{
	IntVec2 m_cursorClientDelta;
	IntVec2 m_cursorClientPosition;

	CursorMode m_cursorMode = CursorMode::POINTER;
};
class InputSystem {
public:
	InputSystem(InputSystemConfig const& inputConfig);
	~InputSystem();
	void StartUp();
	void BeginFrame();
	void Update();
	void EndFrame();
	bool WasKeyJustPressed(unsigned char keyCode);
	bool WasKeyJustReleased(unsigned char keyCode);
	bool IsKeyJustDown(unsigned char keyCode);
	void HandleKeyJustPressed(unsigned char keyCode);
	void HandleKeyJustReleased(unsigned char keyCode);
	XboxController const& GetController(int controllerID);

	static bool Event_KeyPressed(EventArgs& args);

	void SetCurcorMode(CursorMode cursorMode);
	Vec2 GetCursorClientDelta()const;
	Vec2 GetCursorClientPosition()const;
	Vec2 GetCursorNormalizePosition()const;
	CursorMode GetCursorMode()const;
private:
	InputSystemConfig m_config;
	CursorState m_cursorState;

protected:
	XboxController  m_controllers[4];
	KeyButtonState m_keyStates[256];
};