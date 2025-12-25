#include "App.hpp"

#include "GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define WIN32_LEAN_ANDMEAN	
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#if defined(_DEBUG)
#define ENGINE_DEBUG_RENDER
#endif

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

ID3D11Device* m_device = nullptr;
ID3D11DeviceContext* m_deviceContext = nullptr;
IDXGISwapChain* m_swapChain = nullptr;
ID3D11RenderTargetView* m_renderTargetView = nullptr;
ID3D11VertexShader* m_vertexShader = nullptr;
ID3D11PixelShader* m_pixelShader = nullptr;
ID3D11InputLayout* m_InputLayoutForVertex_PCU = nullptr;
ID3D11Buffer* m_vertexBuffer = nullptr;
ID3D11RasterizerState* m_rasterizerState = nullptr;

std::vector<uint8_t> m_vertexShaderByteCode;
std::vector<uint8_t> m_pixelShaderByteCode;

#if defined(ENGINE_DEBUG_RENDER)
void* m_dxgiDebug = nullptr;
void* m_dxgiDebugModule = nullptr;
#endif



Window* g_theWindow=nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;




// #SD1ToDo: This will eventually go away once we add a Window engine class later on.
// 
//constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Startup()
//
void App::Startup()
{

    

	InputSystemConfig inputConfig;
	g_theInput = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = 2.f;
	windowConfig.m_windowTitle = "FirstTriangle";
	windowConfig.m_input = g_theInput;
	g_theWindow = new Window(windowConfig);


	AudioSystemConfig audioConfig;
	g_theAudio = new AudioSystem(audioConfig);

	g_theInput->StartUp();
	g_theWindow->StartUp();
	g_theAudio->Startup();

	//Create Device and Swap Chain
	unsigned int deviceFlags = 0;
#if defined(ENGINE_DEBUG_RENDER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.BufferDesc.Width = g_theWindow->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = g_theWindow->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND)g_theWindow->GetHwnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	
	//Create debug module
#if defined(ENGINE_DEBUG_RENDER)
	m_dxgiDebugModule = (void*)::LoadLibraryA("dxgidebug.dll");
	if (m_dxgiDebugModule == nullptr) {
		ERROR_AND_DIE("Could not load dxgidebug.dll");
	}

	typedef HRESULT(WINAPI* GetDebugModuleCB)(REFIID, void**);
	((GetDebugModuleCB)::GetProcAddress((HMODULE)m_dxgiDebugModule, "DXGIGetDebugInterface"))
		(__uuidof(IDXGIDebug), &m_dxgiDebug);
	if (m_dxgiDebug == nullptr) {
		ERROR_AND_DIE("Could not load debug module");
	}
#endif
	


	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create D3D 11 device and swap chain");
	}

	//Save back buffer view

	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not get swap chain buffer.");
	}

	hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create render target view for swap chain buffer.");
	}

	backBuffer->Release();

	//Add Shader Source
	const char* shaderSource = R"(
    struct vs_input_t
    {
        float3 localPosition : POSITION;
        float4 color : COLOR;
        float2 uv : TEXCOORD;
    };
    
    struct v2p_t
    {
        float4 position : SV_Position;    
        float4 color : COLOR;
        float2 uv : TEXCOORD;
    };
    
    v2p_t VertexMain(vs_input_t input)
    {
        v2p_t v2p;
        v2p.position = float4(input.localPosition, 1);
        v2p.color = input.color;
        v2p.uv = input.uv;
        return v2p;
    };
    
    float4 PixelMain(v2p_t input) : SV_Target0
    {
        return float4(input.color);
    }
    )";

	//Compile vertex shader
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	hr = D3DCompile(
		shaderSource, strlen(shaderSource),
		"VertexShader", nullptr, nullptr,
		"VertexMain", "vs_5_0", shaderFlags,
		0, &shaderBlob, &errorBlob);
	if (SUCCEEDED(hr)) {
		m_vertexShaderByteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			m_vertexShaderByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize());
	}
	else {
		if (errorBlob != NULL) {
			DebuggerPrintf((char*)errorBlob->GetBufferPointer());
		}
		ERROR_AND_DIE(Stringf("Could not compile vertexshader."));
	}
	shaderBlob->Release();
	if (errorBlob != NULL) {
		errorBlob->Release();
	}

	//Create vertex shader

	hr = m_device->CreateVertexShader(
		m_vertexShaderByteCode.data(),
		m_vertexShaderByteCode.size(),
		NULL, &m_vertexShader
	);

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("Could not create vertex shader."));
	}
	//Compile Pixel Shader
	hr = D3DCompile(
		shaderSource,strlen(shaderSource),
		"PixelShader", nullptr, nullptr,
		"PixelMain", "ps_5_0", shaderFlags, 0,
		&shaderBlob, &errorBlob
	);
	if (SUCCEEDED(hr)) {
		m_pixelShaderByteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			m_pixelShaderByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize());
	}
	else {
		if (errorBlob != NULL) {
			DebuggerPrintf((char*)errorBlob->GetBufferPointer());
		}
		ERROR_AND_DIE(Stringf("Could not compile pixel shader"));
	}

	shaderBlob->Release();
	if (errorBlob != NULL) {
		errorBlob->Release();
	}

	//Create Pixel Shader
	hr = m_device->CreatePixelShader(
	    m_pixelShaderByteCode.data(),
		m_pixelShaderByteCode.size(),
		NULL, &m_pixelShader);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create pixel shader."));
	}
	//Create INPUT LAYOUT
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,
		0,0, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
		0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,
		0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	UINT numElements = ARRAYSIZE(inputElementDesc);
	hr = m_device->CreateInputLayout(
	    inputElementDesc, numElements,
		m_vertexShaderByteCode.data(),
		m_vertexShaderByteCode.size(),
		&m_InputLayoutForVertex_PCU
	);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create vertex layout.");
	}

	//triangle
	Vertex_PCU vertices[] = {
		Vertex_PCU(Vec3(-.5f, -.5f, 0.f),Rgba8(255,255,255),Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(0.f, .5f, 0.f),Rgba8(255,255,255),Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(.5f, -.5f, 0.f),Rgba8(255,255,255),Vec2(0.f,0.f))
	};

	//Create vertex buffer
	UINT vertexBufferSize = (UINT)sizeof(vertices);
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = vertexBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_device->CreateBuffer(&bufferDesc, nullptr, &m_vertexBuffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create vertex buffer.");
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, vertices, vertexBufferSize);
	m_deviceContext->Unmap(m_vertexBuffer, 0);

	//Set viewport
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.Width = (float)g_theWindow->GetClientDimensions().x;
	viewport.Height = (float)g_theWindow->GetClientDimensions().y;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	m_deviceContext->RSSetViewports(1, &viewport);

	//Set rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	m_deviceContext->RSSetState(m_rasterizerState);

	//Set Pipeline state
	UINT stride = sizeof(Vertex_PCU);
	UINT startOffset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &startOffset);
	m_deviceContext->IASetInputLayout(m_InputLayoutForVertex_PCU);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Shutdown()
//
void App::Shutdown()
{

	m_rasterizerState->Release();
	m_vertexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_InputLayoutForVertex_PCU->Release();
	m_renderTargetView->Release();
	m_swapChain->Release();
	m_deviceContext->Release();
	m_device->Release();
	//report error leaks and release debug module
#if defined(ENGINE_DEBUG_RENDER)
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		(DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
	);

	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::BeginFrame()
//
void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theAudio->BeginFrame();

}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Update()
//
void App::Update(float deltaSeconds)
{
	g_theInput->Update();
	HandleKeysPressed();
	if (!App::m_isPaused) {
		if (m_isSlowMo) {
			deltaSeconds*=0.1f;
		}
	}
	else {
		deltaSeconds *= 0;
	}
	g_theWindow->Update();


	//handle O key
	if (m_waitingToPause) {
		m_isPaused = true;
		m_waitingToPause = false;
	}

}



//-----------------------------------------------------------------------------------------------
// Some simple OpenGL example drawing code.
// This is the graphical equivalent of printing "Hello, world."
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Render()
// #SD1ToDo: Move *ALL* OpenGL code to RenderContext.cpp (only).
//
// Ultimately this function (App::Render) will only call methods on Renderer (like Renderer::DrawVertexArray)
//	to draw things, never calling OpenGL (nor DirectX) functions directly.
//
void App::Render()
{
	//Clear and present

	//Set render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
	//Clear Screen
	Rgba8 clearColor(127, 127, 127);
	float colorAsFloat[4];
	clearColor.GetAsFloat(colorAsFloat);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colorAsFloat);
	m_deviceContext->Draw(3, 0);
	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	if (hr==DXGI_ERROR_DEVICE_REMOVED||hr==DXGI_ERROR_DEVICE_RESET) {
		ERROR_AND_DIE("Device has been lost, application will now terminate.");
	}

	
}



//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::EndFrame()
//
void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theWindow->EndFrame();

	g_theAudio->EndFrame();

}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::RunFrame()
//
void App::RunFrame()
{
	m_now = GetCurrentTimeSeconds();
	float deltaSeconds = m_now - m_lastFrame;
	m_lastFrame = m_now;
	if (deltaSeconds == 0) {
		deltaSeconds = 16;
	}
	BeginFrame();	// #SD1ToDo: ...becomes just BeginFrame();	once this function becomes App::RunFrame()
	Update(deltaSeconds);		// #SD1ToDo: ...becomes just Update();		once this function becomes App::RunFrame()
	m_now = GetCurrentTimeSeconds();

	Render();		// #SD1ToDo: ...becomes just Render();		once this function becomes App::RunFrame()
	EndFrame();		// #SD1ToDo: ...becomes just EndFrame();	once this function becomes App::RunFrame()
	
}

bool App::HandleKeyPressed(unsigned char keyCode) {
	// #SD1ToDo: Tell the App (or InputSystem later) about this key-pressed event...


	g_theInput->HandleKeyJustPressed(keyCode);
	return 0;
}
bool App::HandleKeyReleased(unsigned char keyCode) {
	XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
	if (controller.IsConnected() && !controller.WasButtonReleased(XBOX_BUTTON_LSTICK)) {
		m_isSlowMo = false;
	}
	else if (keyCode == 'T') // #SD1ToDo: move this "check for ESC pressed" code to App
	{
		m_isSlowMo = false;
	}
	g_theInput->HandleKeyJustReleased(keyCode);
	return 0;
}
bool App::HandleQuitRequested() {
	m_isQuitting = true;
	return 0;
}



void App::HandleKeysPressed() {
	XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
	if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_BACK)) {
		m_isQuitting = true;
	}
	//esc
	else if (g_theInput->WasKeyJustReleased(27)) {
		m_isQuitting = true;
	}
	if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_UP)) {
		m_isPaused = !m_isPaused;
	}
	else if (g_theInput->WasKeyJustReleased('P')) // #SD1ToDo: move this "check for ESC pressed" code to App
	{
		m_isPaused = !m_isPaused;
	}
	if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_LSTICK)) {
		m_isSlowMo = true;
	}
	else if (g_theInput->WasKeyJustPressed('T')) // #SD1ToDo: move this "check for ESC pressed" code to App
	{
		m_isSlowMo = true;
	}
	if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_DOWN)) {
		m_isPaused = false;
		m_waitingToPause = true;
	}
	else if (g_theInput->WasKeyJustPressed('O')) {
		m_isPaused = false;
		m_waitingToPause = true;
	}

}
void App::RunMainloop() {
	// Program main loop; keep running frames until it's time to quit
	while (!IsQuitting())			
	{
		RunFrame(); 
	}
	//Shutdown();
}