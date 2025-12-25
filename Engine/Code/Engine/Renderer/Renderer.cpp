#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include"Engine/Window/Window.hpp"
#include "Engine/Core/Image.hpp"
//#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
//#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
#include "ThirdParty/stb/stb_image.h"
#include "Texture.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/DefaultShader.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
//D11---------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ObjDefinition.hpp"
#include "Engine/Core/ObjUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#if defined(ENGINE_DEBUG_RENDER)
void* m_dxgiDebug = nullptr;
void* m_dxgiDebugModule = nullptr;
#endif

#if defined (OPAQUE)
#undef OPAQUE
#endif

struct CameraConstants {
	Mat44 WorldToCameraTransform;
	Mat44 CameraToRenderTransform;
	Mat44 RenderToClipTransform;
	float camerPosition[3];
	float padding;
};
static const int k_cameraCBO = 2;
static const int k_lightSlot = 1;
static const int k_perFrameSlot = 4;
static const int k_worldSlot = 4;

void Renderer::DrawVertexArray(int numVertex, const Vertex_PCU* vertexes, bool drawIndexed, int numIndexes, unsigned int const* indexes)
{
	VertexBuffer* usingVBO;
	if (m_currentVBO!=nullptr) {
		usingVBO = m_currentVBO;
	}
	else {
		usingVBO = m_defaultVBO;
	}
	Renderer::CopyCPUToGPU(vertexes, numVertex*sizeof(Vertex_PCU), usingVBO);
	if (drawIndexed) {
		//Copy indexes to index buffer
		int size = numIndexes * sizeof(unsigned int);
		if (m_currentIBO->GetSize() < (unsigned int)size) {
			m_currentIBO->Resize(size);
		}
		D3D11_MAPPED_SUBRESOURCE resource;
		m_deviceContext->Map(m_currentIBO->m_indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, indexes, size);
		m_deviceContext->Unmap(m_currentIBO->m_indexBuffer, 0);

		Renderer::DrawIndexedVertexBuffer(usingVBO, m_currentIBO);
	}
	else {
		DrawVertexBuffer(usingVBO,numVertex);
	}
	
};

void Renderer::DrawVertexArray(int numVertex, const Vertex_PCUTBN* vertexes, bool drawIndexed, int numIndexes, unsigned int const* indexes)
{
	VertexBuffer* usingVBO;
	if (m_currentVBO != nullptr) {
		usingVBO = m_currentVBO;
	}
	else {
		usingVBO = m_defaultVBO;
	}
	Renderer::CopyCPUToGPU(vertexes, numVertex * sizeof(Vertex_PCUTBN), usingVBO);
	if (drawIndexed) {
		//Copy indexes to index buffer
		int size = numIndexes * sizeof(unsigned int);
		if (m_currentIBO->GetSize() < (unsigned int)size) {
			m_currentIBO->Resize(size);
		}
		D3D11_MAPPED_SUBRESOURCE resource;
		m_deviceContext->Map(m_currentIBO->m_indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, indexes, size);
		m_deviceContext->Unmap(m_currentIBO->m_indexBuffer, 0);

		Renderer::DrawIndexedVertexBuffer(usingVBO, m_currentIBO);
	}
	else {
		DrawVertexBuffer(usingVBO, numVertex);
	}

};


void Renderer::DrawStaticModel(StaticModel const& model)
{
    BindTexture(model.m_diffuse, 0);
	BindTexture(model.m_normal, 1);
	BindTexture(model.m_sge, 2);

	SetSampleMode(SamplerMode::BILINEAR_WRAP, 0);
	SetSampleMode(SamplerMode::BILINEAR_WRAP, 1);
	SetSampleMode(SamplerMode::BILINEAR_WRAP, 2);

	BindShader(model.m_shader);


	Mat44 modelToWOrld = Mat44::MakeTranslation3D(model.m_position);
	modelToWOrld.Append(model.m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
	SetModelConstants(modelToWOrld, model.m_color);

	DrawIndexedVertexBuffer(model.m_vbo, model.m_ibo);
}

void Renderer::ClearScreen(Rgba8 const& clearColor)
{
	//Clear and present

	//Set render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilDSV);
	//Clear Screen
	float colorAsFloat[4];
	clearColor.GetAsFloat(colorAsFloat);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colorAsFloat);
	m_deviceContext->ClearDepthStencilView(m_depthStencilDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
};

//------------------------------------------------------------------------------------------------
// Given an existing OS Window, create a Rendering Context (RC) for OpenGL or DirectX to draw to it.
// #SD1ToDo: move this to become Renderer::CreateRenderingContext() in Engine/Renderer/Renderer.cpp
//


void Renderer::Startup(){
	//Create Device and Swap Chain
	unsigned int deviceFlags = 0;
#if defined(ENGINE_DEBUG_RENDER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.BufferDesc.Width = m_config.m_window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = m_config.m_window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND)m_config.m_window->GetHwnd();
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


	//Save back buffer view--------------------------------------------------------------------------------------------------------

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
	
	m_defaultShader = CreateShader("Default",m_defaultShaderSource);
	std::string filename = "Data/Shaders/Diffuse.hlsl";
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	m_diffuseShader = CreateShader("Diffuse", buffer.str().c_str(),VertexType::VERTEX_PCUTBN);
	BindShader(m_defaultShader);
	

	m_defaultVBO = CreateVertexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
	m_currentVBO = m_defaultVBO;
	m_currentIBO = nullptr;
	//Camera
	m_cameraCBO = CreateConstantBuffer(sizeof(CameraConstants));
	m_modelCBO = CreateConstantBuffer(sizeof(ModelConstants));
	m_LightCBO = CreateConstantBuffer(sizeof(LightConstants));
	m_PerFrameCBO = CreateConstantBuffer(sizeof(PerFrameConstants));
	m_worldCBO = CreateConstantBuffer(sizeof(WorldConstants));
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

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)RasterizerMode::SOLID_CULL_NONE]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

    rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)RasterizerMode::SOLID_CULL_BACK]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)RasterizerMode::WIREFRAME_CULL_NONE]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)RasterizerMode::WIREFRAME_CULL_BACK]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	
	//BLEND
	D3D11_BLEND_DESC blendDesc = { };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::OPAQUE)]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Can't create blend state opaque");
	}

	blendDesc = { };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::ADDITIVE)]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Can't create blend state additive");
	}

	blendDesc = { };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::ALPHA)]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Can't create blend state alpha");
	}

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[(int)SamplerMode::POINT_CLAMP]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Failed to create POINT_CLAMP sampler state");
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[(int)SamplerMode::BILINEAR_WRAP]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Failed to create BILINEAR_WRAP sampler state");
	}

	Image defaultTextureImage(IntVec2(2, 2), Rgba8(255, 255, 255));
	defaultTextureImage.m_imageFilePath = "Default";
	m_defaultTexture = CreateTextureFromImage(&defaultTextureImage);

	Image defaultNormalImage(IntVec2(2, 2), Rgba8(0, 0, 255));
	defaultNormalImage.m_imageFilePath = "Default";
	m_defaultNormal = CreateTextureFromImage(&defaultNormalImage);

	Image defaultSGEImage(IntVec2(2, 2), Rgba8(255, 255, 0));
	defaultSGEImage.m_imageFilePath = "Default";
	m_defaultSGE = CreateTextureFromImage(&defaultSGEImage);

	//Create depth stencil textuer and view
	D3D11_TEXTURE2D_DESC depthTextureDesc = {};
	depthTextureDesc.Width = m_config.m_window->GetClientDimensions().x;
	depthTextureDesc.Height = m_config.m_window->GetClientDimensions().y;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.SampleDesc.Count = 1;

	hr = m_device->CreateTexture2D(&depthTextureDesc, nullptr, &m_depthStencilTexture);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Coudldn't create texture for depth stencil");
	}
	hr = m_device->CreateDepthStencilView(m_depthStencilTexture, nullptr, &m_depthStencilDSV);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("COudn't create dsv");
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc,
		&m_depthStencilStates[(int)DepthMode::DISABLED]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Failed to create Depth Stencil State");
	}
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)DepthMode::READ_ONLY_ALWAYS]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Failed to create Depth Stencil State");
	}

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)DepthMode::READ_ONLY_LESS_EQUAL]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Failed to create Depth Stencil State");
	}

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)DepthMode::READ_WRITE_LESS_EQUAL]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Failed to create Depth Stencil State");
	}

	BindTexture(nullptr);
	//Set Defaults
	m_desiredBlendMode = BlendMode::ALPHA;
	m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	m_desiredDepthMode = DepthMode::READ_WRITE_LESS_EQUAL;

	IMGUIStartUp();
}
void Renderer::BeginCamera(Camera const& camera){
	CameraConstants cc;
	cc.CameraToRenderTransform = camera.GetCameraToRenderTransform();
	cc.RenderToClipTransform = camera.GetRenderToClipTransform();
	cc.WorldToCameraTransform = camera.GetWorldToCameraTransform();
	cc.camerPosition[0] = camera.GetPosition().x;
	cc.camerPosition[1] = camera.GetPosition().y;
	cc.camerPosition[2] = camera.GetPosition().z;
	CopyCPUToGPU(&cc, sizeof(CameraConstants), m_cameraCBO);
	BindConstantBuffer(k_cameraCBO, m_cameraCBO);

	SetModelConstants();
	AABB2 viewPortSize = camera.m_viewPort;
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = (FLOAT)m_config.m_window->GetClientDimensions().x * viewPortSize.m_mins.x;
	viewport.TopLeftY = (FLOAT)m_config.m_window->GetClientDimensions().y * viewPortSize.m_mins.y;
	viewport.Width    = (FLOAT)m_config.m_window->GetClientDimensions().x * viewPortSize.m_maxs.x;
	viewport.Height   = (FLOAT)m_config.m_window->GetClientDimensions().y * viewPortSize.m_maxs.y;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_deviceContext->RSSetViewports(1, &viewport);

	
}
void Renderer::EndCamera(Camera const& camera){
	UNUSED(camera);
}
void Renderer::Shutdown()
{
    IMGUIShutdown();
	DX_SAFE_RELEASE(m_renderTargetView);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_device);
	for (Shader* shader : m_loadedShaders) {
		delete shader;
		shader = nullptr;
	}
	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	delete m_currentVBO;
	m_currentVBO = nullptr;
	
	
	delete m_modelCBO;
	m_modelCBO = nullptr;
	delete m_LightCBO;
	m_LightCBO = nullptr;
	delete m_PerFrameCBO;
	m_PerFrameCBO = nullptr;
	delete m_currentIBO;
	m_currentIBO = nullptr;
	for (ID3D11BlendState* state : m_blendStates) {
		DX_SAFE_RELEASE(state);
	}
	//DX_SAFE_RELEASE(m_blendState);
	for (ID3D11SamplerState* state : m_samplerStates) {
		DX_SAFE_RELEASE(state);
	}
	for (ID3D11DepthStencilState* state : m_depthStencilStates) {
		DX_SAFE_RELEASE(state);
	}
	for (ID3D11RasterizerState* state : m_rasterizerStates) {
		DX_SAFE_RELEASE(state);
	}
	DX_SAFE_RELEASE(m_depthStencilTexture);
	DX_SAFE_RELEASE(m_depthStencilDSV);
	//DX_SAFE_RELEASE(m_samplerState);
	for (Texture* texture : m_loadedTextures) {
		DX_SAFE_RELEASE(texture->m_texture);
		DX_SAFE_RELEASE(texture->m_shaderResourceView);
	}
	//--------------------------------
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
void Renderer::BeginFrame(){
    IMGUIBeginFrame();
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilDSV);
}
void Renderer::EndFrame(){
    IMGUIEndFrame();
	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	
}

Renderer::Renderer(RendererConfig const& rendererConfig)
:m_config(rendererConfig)
{

}
Renderer::~Renderer() {
    
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFile(char const* imageFilePath)
{
	// See if we already have this texture previously loaded
	Texture* existingTexture = GetTextureForFileName(imageFilePath); // You need to write this
	if (existingTexture)
	{
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	Texture* newTexture = CreateTextureFromFile(imageFilePath);
	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile(char const* imageFilePath)
{
	
	
	/*IntVec2 dimensions = IntVec2(0,0);		// This will be filled in for us to indicate image width & height
	int bytesPerTexel = 0;					// ...and how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)

	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load(1); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* texelData = stbi_load(imageFilePath, &dimensions.x, &dimensions.y, &bytesPerTexel, 0);

	// Check if the load was successful
	GUARANTEE_OR_DIE(texelData, Stringf("Failed to load image \"%s\"", imageFilePath));

	Texture* newTexture = CreateTextureFromData(imageFilePath, dimensions, bytesPerTexel, texelData);

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_free(texelData);
	*/
	Image* img = new Image(imageFilePath);
	

	return CreateTextureFromImage(img);
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData)
{
	// Check if the load was successful
	GUARANTEE_OR_DIE(texelData, Stringf("CreateTextureFromData failed for \"%s\" - texelData was null!", name));
	GUARANTEE_OR_DIE(bytesPerTexel >= 3 && bytesPerTexel <= 4, Stringf("CreateTextureFromData failed for \"%s\" - unsupported BPP=%i (must be 3 or 4)", name, bytesPerTexel));
	GUARANTEE_OR_DIE(dimensions.x > 0 && dimensions.y > 0, Stringf("CreateTextureFromData failed for \"%s\" - illegal texture dimensions (%i x %i)", name, dimensions.x, dimensions.y));

	Texture* newTexture = new Texture();
	newTexture->m_name = name; // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
	newTexture->m_dimensions = dimensions;
	
	return newTexture;
}
Texture* Renderer::CreateTextureFromImage(Image* image) {
	Texture* newTexture = new Texture();
	newTexture->m_name = image->m_imageFilePath.c_str();
	newTexture->m_dimensions = image->m_dimesions;
	
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = image->GetDimensions().x;
	textureDesc.Height = image->GetDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData;
	textureData.pSysMem = image->GetRawData();
	textureData.SysMemPitch = 4 * image->GetDimensions().x;

	HRESULT hr;
	hr = m_device->CreateTexture2D(&textureDesc, &textureData, &newTexture->m_texture);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("CreateTextureFromImage failed for image file \"%s\".", image->GetImageFilePath().c_str()));
	}
	
	hr = m_device->CreateShaderResourceView(newTexture->m_texture, NULL, &newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for image file \"%s\".", image->GetImageFilePath().c_str()));
	}
	m_loadedTextures.push_back(newTexture);
	return newTexture;
}

//-----------------------------------------------------------------------------------------------
void Renderer::BindTexture(Texture* texture, int slot)
{
	if (texture == nullptr) {
	    if(slot==0)
		    m_deviceContext->PSSetShaderResources(0, 1, &m_defaultTexture->m_shaderResourceView);
		if(slot==1)
			m_deviceContext->PSSetShaderResources(1, 1, &m_defaultNormal->m_shaderResourceView);
		if(slot==2)
			m_deviceContext->PSSetShaderResources(2, 1, &m_defaultSGE->m_shaderResourceView);
	}
	else {
		m_currentTexture = texture;
		m_deviceContext->PSSetShaderResources(slot, 1, &m_currentTexture->m_shaderResourceView);
	}
	
}


void Renderer::DrawVertexArray(std::vector<Vertex_PCU> const& verts)
{
	DrawVertexArray(static_cast<int>(verts.size()), verts.data(), false, 0, nullptr);

}

void Renderer::DrawVertexArray(std::vector<Vertex_PCU> const& verts, std::vector<unsigned int> indexes)
{
	DrawVertexArray(static_cast<int>(verts.size()), verts.data(), true, (int)indexes.size(), indexes.data());

}

void Renderer::DrawVertexArray(std::vector<Vertex_PCUTBN> const& verts, std::vector<unsigned int> indexes)
{
	DrawVertexArray(static_cast<int>(verts.size()), verts.data(), true, (int)indexes.size(), indexes.data());

}

Texture* Renderer::GetTextureForFileName(char const* imageFilePath) {
	for(Texture * texture : m_loadedTextures) {
		if (texture->GetImageFilePath() == imageFilePath) {
			return texture;
		}
	}

	return nullptr;
}

BitmapFont* Renderer::CreateOrGetBitmapFont(char const* imageFilePath) {
	std::string str = imageFilePath;
	str += ".png";
	Texture* texture = CreateTextureFromFile(str.c_str());
	BitmapFont* font=new BitmapFont(imageFilePath,*texture);
	return font;
}

StaticModel* Renderer::CreateStaticModel(std::string const& fileName)
{
    ObjDefinition def(fileName);
	StaticModel* model = new StaticModel();

	if (def.m_diffuse.compare("") != 0) {
		model->m_diffuse = CreateOrGetTextureFromFile(def.m_diffuse.c_str());
	}

	if (def.m_normal.compare("") != 0) {
		model->m_normal = CreateOrGetTextureFromFile(def.m_normal.c_str());
	}

	if (def.m_sge.compare("") != 0) {
		model->m_sge = CreateOrGetTextureFromFile(def.m_sge.c_str());
	}
	else {
		model->m_sge=CreateOrGetTextureFromFile("Data/Obj/DefaultSGE.png");
	}
	
	if (def.m_shader.compare("diffuse") == 0) {
		model->m_shader = m_diffuseShader;
	}
	else {
		model->m_shader = m_defaultShader;
	}

	model->m_vbo = CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	model->m_ibo = CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	std::vector<Vertex_PCUTBN> verts;
	std::vector<unsigned int> indexes;
	ParseObjFromFile(def.m_objFilePath, verts, indexes, def.m_reverseTriangle);

	TransformVertexArray3D(verts, def.m_modelToWorldOrigin);

	CopyIndexedVertexCPUTBNToBuffers(verts, indexes, model->m_vbo, model->m_ibo);

	return model;
}

void Renderer::SetBlendMode(BlendMode blendMode)
{
	m_desiredBlendMode = blendMode;
	
}

Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource, VertexType vertexType) {
	ShaderConfig shaderConfig;
	shaderConfig.m_name = shaderName;
	Shader* shader=new Shader(shaderConfig);
	std::vector<unsigned char> vertexByteCode;
	std::vector<unsigned char> pixelByteCode;
	CompileShaderToByteCode(vertexByteCode, shader->m_config.m_name.c_str(), shaderSource, shader->m_config.m_vertexEntryPoint.c_str(), "vs_5_0");
	HRESULT hr;
	hr = m_device->CreateVertexShader(
		vertexByteCode.data(),
		vertexByteCode.size(),
		NULL, &shader->m_vertexShader
	);

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("Could not create vertex shader."));
	}
	CompileShaderToByteCode(pixelByteCode, shader->m_config.m_name.c_str(), shaderSource, shader->m_config.m_pixelEntryPoint.c_str(), "ps_5_0");
	hr = m_device->CreatePixelShader(
		pixelByteCode.data(),
		pixelByteCode.size(),
		NULL, &shader->m_pixelShader
	);

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("Could not create pixel shader."));
	}

	//Create INPUT LAYOUT
	
	if (vertexType == VertexType::VERTEX_PCU) {
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
			vertexByteCode.data(),
			vertexByteCode.size(),
			&shader->m_InputLayout
		);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Could not create vertex layout.");
		}
		m_currentShader = shader;
		m_loadedShaders.push_back(shader);
		return shader;
	}
	else {
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		    {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA,0},
		    {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,0, D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		    {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
		    {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	        {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		UINT numElements = ARRAYSIZE(inputElementDesc);
		hr = m_device->CreateInputLayout(
			inputElementDesc, numElements,
			vertexByteCode.data(),
			vertexByteCode.size(),
			&shader->m_InputLayout
		);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Could not create vertex layout.");
		}
		m_currentShader = shader;
		m_loadedShaders.push_back(shader);
		return shader;
	}

	
	
}

//Read source from file ver.
Shader* Renderer::CreateShader(char const* shaderName, VertexType vertexType) {
	
	std::string outString;
	FileReadToString(outString, "Data/Shader/Default.hlsl");
	return CreateShader(shaderName, outString.c_str(), vertexType);

}


bool    Renderer::CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target) {
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	HRESULT hr;
	
	hr = D3DCompile(
		source, strlen(source),
		name, nullptr, nullptr,
		entryPoint, target, shaderFlags,
		0, &shaderBlob, &errorBlob);
	
	
	if (SUCCEEDED(hr)) {
		outByteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			outByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize());
	}
	else {
		if (errorBlob != NULL) {
			char const* errorText = (char*)errorBlob->GetBufferPointer();
			DebuggerPrintf( errorText );
			ERROR_AND_DIE( Stringf( "Shader %s failed to compile.  Error is:\n%s", name, errorText ) );
		}
		else
		{
			ERROR_AND_DIE( Stringf( "Shader %s failed to compile.  No error text available.", name ) );
		}
	}
	shaderBlob->Release();
	if (errorBlob != NULL) {
		errorBlob->Release();
	}
	return true;
}


void    Renderer::BindShader(Shader* shader) {
	if (shader == nullptr) {
		shader = m_defaultShader;
	}
	m_deviceContext->IASetInputLayout(shader->m_InputLayout);
	m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);
}

VertexBuffer* Renderer::CreateVertexBuffer(const unsigned int size, unsigned int stride=sizeof(Vertex_PCU)) {
	VertexBuffer* vbo = new VertexBuffer(m_device, size, stride);
	return vbo;
}

IndexBuffer* Renderer::CreateIndexBuffer(const unsigned int size, unsigned int stride) {
	IndexBuffer* ibo = new IndexBuffer(m_device, size, stride);
	if(!m_currentIBO)
	    m_currentIBO = ibo;
	return ibo;
}

void Renderer::CopyIndexedVertexCPUTBNToBuffers(std::vector<Vertex_PCUTBN> const& verts, std::vector<unsigned int> const& indexes, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
{
    
	int numVertex = static_cast<int>(verts.size());
	const Vertex_PCUTBN* vertexes = verts.data();
	SetBuffers(vertexBuffer, indexBuffer);
	int numIndexes = (int)indexes.size();
	unsigned int const* indexesData = indexes.data();

	CopyCPUToGPU(vertexes, numVertex * sizeof(Vertex_PCUTBN), vertexBuffer);

	//Copy indexes to index buffer
	int size = numIndexes * sizeof(unsigned int);
	//-----------------------
	if (indexBuffer->GetSize() < (unsigned int)size) {
		
	}
	indexBuffer->Resize(size);
	//-------------------------------
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(indexBuffer->m_indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, indexesData, size);
	m_deviceContext->Unmap(indexBuffer->m_indexBuffer, 0);
}

void          Renderer::CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo) {
	//---------------------------
	if (vbo->GetSize() < size) {
		//vbo->Resize(size);
	}
	//------------------
	vbo->Resize(size);
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(vbo->m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(vbo->m_vertexBuffer, 0);
}


void          Renderer::BindVertexBuffer(VertexBuffer* vbo) {
	UINT stride = vbo->GetStride();
	UINT startOffset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &vbo->m_vertexBuffer, &stride, &startOffset);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::IMGUIStartUp()
{
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io.DisplaySize.x=1000;
	io.DisplaySize.y=1000;
	ImGui_ImplDX11_Init(m_device, m_deviceContext);
}

void Renderer::IMGUIBeginFrame()
{

	ImGui_ImplDX11_NewFrame();

	ImGui::NewFrame();
}

void Renderer::IMGUIEndFrame()
{
    ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();

}

void Renderer::IMGUIShutdown()
{
    ImGui_ImplDX11_Shutdown();

    ImGui::DestroyContext();
}

ConstantBuffer* Renderer::CreateConstantBuffer(const unsigned int size) {
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBuffer* cbo=new ConstantBuffer(size);
	HRESULT hr;
	hr = m_device->CreateBuffer(&bufferDesc, nullptr, &cbo->m_buffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create constant buffer.");
	}
	return cbo;
}
void            Renderer::CopyCPUToGPU(const void* data, unsigned int size, ConstantBuffer* cbo) {
	
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(cbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(cbo->m_buffer, 0);
}
void            Renderer::BindConstantBuffer(int slot, ConstantBuffer* cbo) {
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_buffer);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

void Renderer::DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount) {
	BindVertexBuffer(vbo);
	SetStatesIfChanged();
	m_deviceContext->Draw(vertexCount, 0);
}
void Renderer::DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo) {
	BindVertexBuffer(vbo);
	m_deviceContext->IASetIndexBuffer(ibo->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	SetStatesIfChanged();
	m_deviceContext->DrawIndexed(ibo->GetSize()/ibo->m_stride, 0, 0);
}

void Renderer::SetStatesIfChanged() {
	if (m_blendStates[(int)m_desiredBlendMode] != m_blendState) {
		
		m_blendState = m_blendStates[(int)m_desiredBlendMode];
		float blendFactor[4] = { 0.f,0.f,0.f,0.f };
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState(m_blendState, blendFactor, sampleMask);
		
	}
	/*if (m_samplerStates[(int)m_desiredSamplerMode] != m_samplerState) {
		m_samplerState = m_samplerStates[(int)m_desiredSamplerMode];
		m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	}*/
	if (m_rasterizerStates[(int)m_desiredRasterizerMode]!= m_rasterizerState) {
		m_rasterizerState = m_rasterizerStates[(int)m_desiredRasterizerMode];
		m_deviceContext->RSSetState(m_rasterizerState);
	}
	if (m_depthStencilStates[(int)m_desiredDepthMode] != m_depthStencilState) {
		m_depthStencilState = m_depthStencilStates[(int)m_desiredDepthMode];
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	}
}



void Renderer::SetSampleMode(SamplerMode sampleMode, int slot) {
	m_samplerState = m_samplerStates[(int)sampleMode];
	m_deviceContext->PSSetSamplers(slot, 1, &m_samplerState);
}


void Renderer::SetRasterizerMode(RasterizerMode rasterizerMode) {
	m_desiredRasterizerMode = rasterizerMode;
}
void Renderer::SetDepthMode(DepthMode depthMode) {
	m_desiredDepthMode = depthMode;
}

void Renderer:: SetModelConstants(const Mat44& modelToWOrldTransform, const Rgba8& modelColor) {
	ModelConstants mc;
	mc.ModelToWorldTransform = modelToWOrldTransform;
	mc.ModelColor[0] = (float)modelColor.r/255.f;
	mc.ModelColor[1] = (float)modelColor.g/255.f;
	mc.ModelColor[2] = (float)modelColor.b/255.f;
	mc.ModelColor[3] = (float)modelColor.a/255.f;
	CopyCPUToGPU(&mc, sizeof(ModelConstants), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);
}

void Renderer::SetLightConstants(Vec3 direction, float sunIntensity, float ambientIntensity, Rgba8 const& sunColor, std::vector<LightSource> sources) {
    
	m_lc.SunDirection[0] = direction.GetNormalized().x;
	m_lc.SunDirection[1] = direction.GetNormalized().y;
	m_lc.SunDirection[2] = direction.GetNormalized().z;
	m_lc.SunIntensity = sunIntensity;
	m_lc.AmbientIntensity = ambientIntensity;
	m_lc.SunColor[0] = (float)sunColor.r / 255.f;
	m_lc.SunColor[1] = (float)sunColor.g / 255.f;
	m_lc.SunColor[2] = (float)sunColor.b / 255.f;
	sources.resize(8);
	m_lc.LightSource1 = sources[0];
	m_lc.LightSource2 = sources[1];
	m_lc.LightSource3 = sources[2];
	m_lc.LightSource4 = sources[3];
	m_lc.LightSource5 = sources[4];
	m_lc.LightSource6 = sources[5];
	m_lc.LightSource7 = sources[6];
	m_lc.LightSource8 = sources[7];
	CopyCPUToGPU(&m_lc, sizeof(LightConstants), m_LightCBO);
	BindConstantBuffer(k_lightSlot, m_LightCBO);
}

void Renderer::SetWorldConstants(WorldConstants wc)
{
    CopyCPUToGPU(&wc, sizeof(WorldConstants), m_worldCBO);
	BindConstantBuffer(k_worldSlot, m_worldCBO);
}

LightSource Renderer::CreateSpotLight(float intensity, float innerCircle, float outterCircle,Vec3 position, Rgba8 const& color,
Vec3 direction, float innerPenumbraDeg, float outterPenumbraDeg)
{
    LightSource source;
	source.SourcePosition[0] = position.x;
	source.SourcePosition[1] = position.y;
	source.SourcePosition[2] = position.z;
	source.Intensity = intensity;
	source.InnerRadius = innerCircle;
	source.OutterRadius = outterCircle;
	source.Direction[0] = direction.x;
	source.Direction[1] = direction.y;
	source.Direction[2] = direction.z;
	if(innerPenumbraDeg==360)
	    source.InnerPenumbra = -1;
	else
	    source.InnerPenumbra = DotProduct2D(Vec2(1.f,0.f), Vec2::MakeFromPolarDegrees(innerPenumbraDeg));
	if(outterPenumbraDeg ==360)
	    source.OutterPenumbra = -2;
	else
	    source.OutterPenumbra = DotProduct2D(Vec2(1.f, 0.f), Vec2::MakeFromPolarDegrees(outterPenumbraDeg));
	source.Color[0] = (float)color.r / 255.f;
	source.Color[1] = (float)color.g/255.f;
	source.Color[2] =(float)color.b/255.f;
	return source;
}

void Renderer::SetPerFrameConstants(float time, int c_int, float c_float)
{
    PerFrameConstants pc;
	pc.time = time;
	pc.c_int = c_int;
	pc.c_float = c_float;
	CopyCPUToGPU(&pc, sizeof(PerFrameConstants),m_PerFrameCBO);
	BindConstantBuffer(k_perFrameSlot, m_PerFrameCBO);
}

void Renderer::SetBuffers(VertexBuffer* vbo, IndexBuffer* ibo) {
	if (vbo == nullptr) {
		m_currentVBO = m_defaultVBO;
		m_currentIBO = nullptr;

	}
	else {
		m_currentVBO = vbo;
		m_currentIBO = ibo;
	}
}



void Renderer::Set2DDefault() {
	SetBlendMode(BlendMode::OPAQUE);
	SetDepthMode(DepthMode::DISABLED);
	SetModelConstants();
	SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
}
