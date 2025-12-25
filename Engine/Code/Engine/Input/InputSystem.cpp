#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include<Windows.h>

//------------
unsigned char const KEYCODE_F1 = VK_F1;
unsigned char const KEYCODE_F2 = VK_F2;
unsigned char const KEYCODE_F3 = VK_F3;
unsigned char const KEYCODE_F4 = VK_F4;
unsigned char const KEYCODE_F5 = VK_F5;
unsigned char const KEYCODE_F6 = VK_F6;
unsigned char const KEYCODE_F7 = VK_F7;
unsigned char const KEYCODE_F8 = VK_F8;
unsigned char const KEYCODE_F9 = VK_F9;
unsigned char const KEYCODE_F10 = VK_F10;
unsigned char const KEYCODE_F11 = VK_F11;
unsigned char const KEYCODE_F12 = VK_F12;
unsigned char const KEYCODE_UP = VK_UP;
unsigned char const KEYCODE_DOWN = VK_DOWN;
unsigned char const KEYCODE_LEFT = VK_LEFT;
unsigned char const KEYCODE_RIGHT = VK_RIGHT;
unsigned char const KEYCODE_LEFT_MOUSE = VK_LBUTTON;
unsigned char const KEYCODE_RIGHT_MOUSE = VK_RBUTTON;
unsigned char const KEYCODE_DEV_CONSOLE = VK_OEM_3;
unsigned char const KEYCODE_LEFTBRACKET = 0xDB;
unsigned char const KEYCODE_RIGHTBRACKET = 0xDD;
unsigned char const KEYCODE_TILDE = 0xC0;
unsigned char const KEYCODE_BACKSPACE = VK_BACK;
unsigned char const KEYCODE_INSERT = VK_INSERT;
unsigned char const KEYCODE_DELETE = VK_DELETE;
unsigned char const KEYCODE_HOME = VK_HOME;
unsigned char const KEYCODE_END   = VK_END;
unsigned char const KEYCODE_ENTER = VK_RETURN;
unsigned char const KEYCODE_ESCAPE = VK_ESCAPE;
unsigned char const KEYCODE_SHIFT = VK_SHIFT;
unsigned char const KEYCODE_EQUAL = VK_OEM_PLUS;
unsigned char const KEYCODE_CONTROL = VK_CONTROL;
unsigned char const KEYCODE_DECIMAL = VK_DECIMAL;
extern InputSystem* g_theInput;

//-------
InputSystem::InputSystem(InputSystemConfig const& inputConfig) 
:m_config(inputConfig)
{

}
InputSystem::~InputSystem() {

}

void InputSystem::StartUp() {
	for (int i = 0; i < 256; i++) {
		m_keyStates[i] = KeyButtonState();
		m_keyStates[i].m_wasPressedLastFrame = false;
		m_keyStates[i].m_isPressed = false;
	}
	for (int i = 0; i < 4; i++) {
		m_controllers[i]=XboxController();
	}
}
void InputSystem::BeginFrame() {
	CURSORINFO pci;
	pci.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&pci);
	bool visible = (pci.flags & CURSOR_SHOWING);
	if (m_cursorState.m_cursorMode==CursorMode::POINTER) {
		if (!visible) {
			//set cursor visible
			int i = ShowCursor(true);
			while (i < 0) {
				i = ShowCursor(true);
			}
		}
	}
	else {
		if (visible) {
			//hide cursor
			int i = ShowCursor(false);
			while (i >= 0) {
				i = ShowCursor(false);
			}
		}
	}
	POINT lpPoint;
	GetCursorPos(&lpPoint);
	Vec2 lastFramePos = m_cursorState.m_cursorClientPosition;
	m_cursorState.m_cursorClientPosition = IntVec2(lpPoint.x,lpPoint.y);

	if (m_cursorState.m_cursorMode == CursorMode::FPS) {
		Vec2 delta = GetCursorClientPosition() - lastFramePos;;
		m_cursorState.m_cursorClientDelta = IntVec2((int)delta.x, (int)delta.y);
		SetCursorPos(800, 400);
		lpPoint;
		GetCursorPos(&lpPoint);
		m_cursorState.m_cursorClientPosition = IntVec2((int)lpPoint.x, (int)lpPoint.y);
	}
	else {
		m_cursorState.m_cursorClientDelta = IntVec2(0, 0);
	}
}
void InputSystem::Update() {
	for (int i = 0; i < 4; i++) {
		m_controllers[i].m_id = i;
		m_controllers[i].Update();
	}

}
void InputSystem::EndFrame() {
	for (int i = 0; i < 256;i++) {
		m_keyStates[i].m_wasPressedLastFrame = m_keyStates[i].m_isPressed;
	}
}
bool InputSystem::WasKeyJustPressed(unsigned char keyCode){
	if (g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL) {
		return false;
	}
	return m_keyStates[keyCode].m_wasPressedLastFrame;
}
bool InputSystem::WasKeyJustReleased(unsigned char keyCode) {
	if (g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL) {
		return false;
	}
	return m_keyStates[keyCode].m_wasPressedLastFrame && !m_keyStates[keyCode].m_isPressed;
}
bool InputSystem::IsKeyJustDown(unsigned char keyCode) {
	if (g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL) {
		return false;
	}
	return m_keyStates[keyCode].m_isPressed;
}
void InputSystem::HandleKeyJustPressed(unsigned char keyCode) {
	m_keyStates[keyCode].m_isPressed = true;
}
void InputSystem::HandleKeyJustReleased(unsigned char keyCode) {
	m_keyStates[keyCode].m_isPressed = false;

}
XboxController const& InputSystem::GetController(int controllerID) {
	return m_controllers[controllerID];

}
bool InputSystem::Event_KeyPressed(EventArgs& args) {
	if (!g_theInput) {
		return false;
	}
	unsigned char keyCode = (unsigned char)args.GetValue("KeyCode", -1);
	g_theInput->HandleKeyJustPressed(keyCode);
	return true;
}

void InputSystem::SetCurcorMode(CursorMode cursorMode) {
	m_cursorState.m_cursorMode = cursorMode;
}
Vec2 InputSystem::GetCursorClientDelta()const {
	return m_cursorState.m_cursorClientDelta;
}
Vec2 InputSystem::GetCursorClientPosition()const {
	return m_cursorState.m_cursorClientPosition;
}
Vec2 InputSystem::GetCursorNormalizePosition()const {
	return GetCursorClientPosition().GetNormalized();
}

CursorMode InputSystem::GetCursorMode()const {
	return m_cursorState.m_cursorMode;
}