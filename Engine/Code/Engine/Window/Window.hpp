#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include"Engine/Math/IntVec2.hpp"
#include <string>
#include "Engine/Core/NamedStrings.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/backends/imgui_impl_dx11.h"
#include "ThirdParty/imgui/backends/imgui_impl_win32.h"
typedef NamedStrings EventArgs;
class InputSystem;
//----------------------
struct WindowConfig {
	InputSystem* m_input = nullptr;
	float m_aspectRatio = 16/9;
	std::string m_windowTitle = "Untitled App";
	bool m_fullScreen = false;
};

//-------------

class Window {
public:
	Window(WindowConfig const& config);
	~Window();
	void StartUp();
	void BeginFrame();
	void Update();
	void EndFrame();
	void ShutDown();
	WindowConfig const& GetConfig() const;
	static Window* s_mainWindow;

	void* GetDisplayContext()const;
	Vec2 GetNormalizedMouseUV()const;

	void* GetHwnd()const;
	IntVec2 GetClientDimensions()const;
	bool HasFocus();

	void* m_windowHandle = nullptr;
	IntVec2 m_clientDimensions;

	float GetAspectRatio();
private:
	void RunMessagePump();
	void CreateOSWindow();
	//void* m_windowHandle;
	WindowConfig m_config;
	void* m_displayDeviceContext = nullptr;
	
	
	
};