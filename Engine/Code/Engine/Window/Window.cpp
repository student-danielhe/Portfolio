#include"Engine/Window/Window.hpp"
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include"Engine/Core/ErrorWarningAssert.hpp"
#include"Engine/Input/InputSystem.hpp"
#include "Engine/Core/EventSystem.hpp"

Window* Window::s_mainWindow = nullptr;

Window::Window(WindowConfig const& config)
	:m_config(config)
{
	s_mainWindow = this;
}
Window::~Window() {

}
void Window::StartUp() {
	CreateOSWindow();

	g_theEventSystem->SubscribeEventCallbackFunction("KeyPressed", InputSystem::Event_KeyPressed);

}
void Window::BeginFrame() {
	RunMessagePump();
	ImGui_ImplWin32_NewFrame();
}
void Window::Update() {

}
void Window::EndFrame() {

}

void Window::ShutDown()
{
    ImGui_ImplWin32_Shutdown();
}

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called back by Windows whenever we tell it to (by calling DispatchMessage).

LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	Window::s_mainWindow->m_windowHandle = windowHandle;
	InputSystem* input = nullptr;

	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(windowHandle, wmMessageCode, wParam, lParam))
		return true;

	if (Window::s_mainWindow) {
		WindowConfig config = Window::s_mainWindow->GetConfig();
		input = config.m_input;
	}
	switch (wmMessageCode)
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{
		FireEvent("Quit");
		//return 0;
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		if (input) {
			//unsigned char asKey = (unsigned char)wParam;
			//input->HandleKeyJustPressed(asKey);
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
			FireEvent("KeyPressed", args);
			FireEvent("KeyPressedForDev", args);
			
			
		}
		return 0;
		
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		if (input) {
			unsigned char asKey = (unsigned char)wParam;
			input->HandleKeyJustReleased(asKey);
		}
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		if (input)
		{
			input->HandleKeyJustPressed(KEYCODE_LEFT_MOUSE);
		}
		return 0;
	}
	case WM_LBUTTONUP:
	{
		if (input)
		{
			input->HandleKeyJustReleased(KEYCODE_LEFT_MOUSE);
		}
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		if (input)
		{
			input->HandleKeyJustPressed(KEYCODE_RIGHT_MOUSE);
		}
		return 0;
	}
	case WM_RBUTTONUP:
	{
		if (input)
		{
			input->HandleKeyJustReleased(KEYCODE_RIGHT_MOUSE);
		}
		return 0;
	}
	// #SD1ToDo: Tell the App (or InputSystem later) about this key-released event...
	break;
	}
	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

//-----------------------------------------------------------------------------------------------
void Window::CreateOSWindow()
{
	HMODULE applicationInstanceHandle = ::GetModuleHandle(NULL);
	float clientAspect = m_config.m_aspectRatio;
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	

	if (m_config.m_fullScreen) {
		DWORD const windowStyleFlags = WS_POPUP;
		DWORD const windowStyleExFlags = WS_EX_APPWINDOW;
		// Get desktop rect, dimensions, aspect
		RECT desktopRect;
		HWND desktopWindowHandle = GetDesktopWindow();
		GetClientRect(desktopWindowHandle, &desktopRect);
		float desktopWidth = (float)(desktopRect.right - desktopRect.left);
		float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
		float desktopAspect = desktopWidth / desktopHeight;
		clientAspect = desktopAspect;
		m_config.m_aspectRatio = clientAspect;
		RECT clientRect;
		clientRect.left = desktopRect.left;
		clientRect.right = desktopRect.right;
		clientRect.top = desktopRect.top;
		clientRect.bottom = desktopRect.bottom;


		// Calculate the outer dimensions of the physical window, including frame et. al.
		RECT windowRect = clientRect;
		AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

		WCHAR windowTitle[1024];
		MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
		HWND windowHandle = CreateWindowEx(
			windowStyleExFlags,
			windowClassDescription.lpszClassName,
			windowTitle,
			windowStyleFlags,
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			(HINSTANCE)applicationInstanceHandle,
			NULL);
		m_windowHandle = windowHandle;
		m_clientDimensions = IntVec2(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
		ShowWindow(windowHandle, SW_SHOW);
		SetForegroundWindow(windowHandle);
		SetFocus(windowHandle);

		m_displayDeviceContext = GetDC(windowHandle);

		HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
		SetCursor(cursor);
	}
	else {
		DWORD const windowStyleFlags = WS_CAPTION | WS_BORDER | /*WS_THICKFRAME |*/ WS_SYSMENU | WS_OVERLAPPED;
		DWORD const windowStyleExFlags = WS_EX_APPWINDOW;
		// Get desktop rect, dimensions, aspect
		RECT desktopRect;
		HWND desktopWindowHandle = GetDesktopWindow();
		GetClientRect(desktopWindowHandle, &desktopRect);
		float desktopWidth = (float)(desktopRect.right - desktopRect.left);
		float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
		float desktopAspect = desktopWidth / desktopHeight;

		// Calculate maximum client size (as some % of desktop size)
		constexpr float maxClientFractionOfDesktop = 0.90f;
		float clientWidth = desktopWidth * maxClientFractionOfDesktop;
		float clientHeight = desktopHeight * maxClientFractionOfDesktop;
		if (clientAspect > desktopAspect)
		{
			// Client window has a wider aspect than desktop; shrink client height to match its width
			clientHeight = clientWidth / clientAspect;
		}
		else
		{
			// Client window has a taller aspect than desktop; shrink client width to match its height
			clientWidth = clientHeight * clientAspect;
		}
		
		// Calculate client rect bounds by centering the client area
		float clientMarginX = 0.5f * (desktopWidth - clientWidth);
		float clientMarginY = 0.5f * (desktopHeight - clientHeight);
		RECT clientRect;
		clientRect.left = (int)clientMarginX;
		clientRect.right = clientRect.left + (int)clientWidth;
		clientRect.top = (int)clientMarginY;
		clientRect.bottom = clientRect.top + (int)clientHeight;

		// Calculate the outer dimensions of the physical window, including frame et. al.
		RECT windowRect = clientRect;
		AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

		WCHAR windowTitle[1024];
		MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
		HWND windowHandle = CreateWindowEx(
			windowStyleExFlags,
			windowClassDescription.lpszClassName,
			windowTitle,
			windowStyleFlags,
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			(HINSTANCE)applicationInstanceHandle,
			NULL);
		m_windowHandle = windowHandle;
		m_clientDimensions = IntVec2(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
		ShowWindow(windowHandle, SW_SHOW);
		SetForegroundWindow(windowHandle);
		SetFocus(windowHandle);

		m_displayDeviceContext = GetDC(windowHandle);

		HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
		SetCursor(cursor);
	}

	
}

WindowConfig const& Window::GetConfig() const {
	return m_config;
}
void* Window::GetDisplayContext()const {
	return m_displayDeviceContext;
}
void Window::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		BOOL const wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}
Vec2 Window::GetNormalizedMouseUV()const {
	HWND windowHandle = static_cast<HWND>(m_windowHandle);
	POINT cursorCoord;
	RECT clientRect;
	::GetCursorPos(&cursorCoord);
	::ScreenToClient(windowHandle, &cursorCoord);
	::GetClientRect(windowHandle, &clientRect);
	float cursorX = static_cast<float>(cursorCoord.x) / static_cast<float>(clientRect.right);
	float cursorY = static_cast<float>(cursorCoord.y) / static_cast<float>(clientRect.bottom);
	return Vec2(cursorX, 1.f - cursorY);
}
void* Window::GetHwnd()const {
	return m_windowHandle;
}
IntVec2 Window::GetClientDimensions()const {
	return m_clientDimensions;
}

bool Window::HasFocus() {
	return m_windowHandle == GetActiveWindow();
}

float Window::GetAspectRatio()
{
    return m_config.m_aspectRatio;
}
