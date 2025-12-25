#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include <vector>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine//Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "ConstantBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/StaticModel.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/backends/imgui_impl_dx11.h"
#include "ThirdParty/imgui/backends/imgui_impl_win32.h"
#define DX_SAFE_RELEASE(dxObject) {if ((dxObject) != nullptr) {(dxObject)->Release();(dxObject) = nullptr;}}
#if defined (OPAQUE)
#undef OPAQUE
#endif
enum class VertexType {
	VERTEX_PCU,
	VERTEX_PCUTBN,
	COUNT
};
//using namespace std;
class Window;
class Texture;
class Image;
struct RendererConfig {
	Window* m_window = nullptr;
};

enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQUE,
	COUNT
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	COUNT
};

enum class RasterizerMode 
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

enum class DepthMode 
{
	DISABLED,
	READ_ONLY_ALWAYS,
	READ_ONLY_LESS_EQUAL,
	READ_WRITE_LESS_EQUAL,
	COUNT
};
struct LightSource
{
	float SourcePosition[3];
	float padding = 0.f;
	float Direction[3];
	float padding2 = 0.f;
	float Color[3];
	float Intensity = 0.f;
	float InnerRadius = 1.f;
	float OutterRadius = 2.f;
	float InnerPenumbra = -1.f;
	float OutterPenumbra = -2.f;
};
struct ModelConstants
{
	Mat44 ModelToWorldTransform;
	float ModelColor[4];
};
struct LightConstants {
	float SunDirection[3];
	float SunIntensity;
	float AmbientIntensity;
	float SunColor[3];
	LightSource LightSource1;
	LightSource LightSource2;
	LightSource LightSource3;
	LightSource LightSource4;
	LightSource LightSource5;
	LightSource LightSource6;
	LightSource LightSource7;
	LightSource LightSource8;
};
struct PerFrameConstants {
	float time = 0.f;
	int c_int = 0;
	float c_float = 0.f;
	float padding;
};

struct WorldConstants
{
	Vec4 CameraPosition;
	Vec4 IndoorLightColor;
	Vec4 OutdoorLightColor;
	Vec4 SkyColor;
	float FogNearDistance;
	float FogFarDistance;
	Vec2 Padding;
};


static const int k_modelConstantsSlot = 3;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11DepthStencilState;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
class Renderer
{
public:
	Renderer(RendererConfig const& rendererConfig);
	~Renderer();
	void Startup();
	void BeginFrame();
	void EndFrame(); 
	void Shutdown();

	void ClearScreen(Rgba8 const& clearColor);
	void BeginCamera(Camera const& camera);
	void EndCamera(Camera const& camera);
	void DrawVertexArray(int numVertex, Vertex_PCU const* vertexes, bool drawindexed = false, int numIndexes=0, unsigned int const* indexes=nullptr);
	void DrawVertexArray(int numVertex, Vertex_PCUTBN const* vertexes, bool drawindexed = false, int numIndexes = 0, unsigned int const* indexes = nullptr);

	//------------------------------------------------------------------------------------------------
	Texture* CreateOrGetTextureFromFile(char const* imageFilePath);
	BitmapFont* CreateOrGetBitmapFont(char const* imageFilePath);
	StaticModel* CreateStaticModel(std::string const& fileName);

	Texture* CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData);
	void BindTexture(Texture* texture, int slot=0);
	void SetSampleMode(SamplerMode sampleMode, int slot=0);

	void DrawVertexArray(std::vector<Vertex_PCU> const& verts);
	void DrawVertexArray(std::vector<Vertex_PCU> const& verts,  std::vector<unsigned int> indexes);
	void DrawVertexArray(std::vector<Vertex_PCUTBN> const& verts, std::vector<unsigned int> indexes);

	void DrawStaticModel(StaticModel const& model);

	Texture* GetTextureForFileName(char const* imageFilePath);
	void SetBlendMode(BlendMode blendMode);

	Shader* CreateShader(char const* shaderName, char const* shaderSource, VertexType vertexType=VertexType::VERTEX_PCU);
	Shader* CreateShader(char const* shaderName, VertexType vertexType = VertexType::VERTEX_PCU);
	bool CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target);
	void BindShader(Shader* shader);
	void SetModelConstants(
		const Mat44& modelToWOrldTransform = Mat44(),
		const Rgba8& modelColor = Rgba8(255, 255, 255));

    void SetWorldConstants(WorldConstants wc);

	void Set2DDefault();

	VertexBuffer* CreateVertexBuffer(const unsigned int size, unsigned int stride);
	IndexBuffer* CreateIndexBuffer(const unsigned int size = 0, unsigned int stride = sizeof(unsigned char));

	void DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount);
	void DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo);

	void CopyIndexedVertexCPUTBNToBuffers(
	std::vector<Vertex_PCUTBN> const& verts, std::vector<unsigned int> const& indexes,
	VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer);

private:
	//functions
	Texture* CreateTextureFromFile(char const* imageFilePath);
	Texture* CreateTextureFromImage(Image* image);
	const Texture* m_defaultTexture = nullptr;
	const Texture* m_defaultNormal = nullptr;
	const Texture* m_defaultSGE = nullptr;
private:
	RendererConfig m_config;
	std::vector<Texture*> m_loadedTextures;
	Texture* m_currentTexture;
public:
	Shader* m_defaultShader = nullptr;
	Shader* m_diffuseShader = nullptr;

	LightConstants m_lc = LightConstants();
protected:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	
	std::vector<Shader*> m_loadedShaders;
	Shader* m_currentShader = nullptr;
	VertexBuffer* m_defaultVBO = nullptr;
	VertexBuffer* m_currentVBO = nullptr;
	ConstantBuffer* m_cameraCBO = nullptr;
	ConstantBuffer* m_modelCBO = nullptr;
	ConstantBuffer* m_LightCBO = nullptr;
	ConstantBuffer* m_PerFrameCBO = nullptr;
	IndexBuffer* m_currentIBO = nullptr;
	ConstantBuffer* m_worldCBO = nullptr;
	//---------------------------------------------------------------
	ID3D11BlendState* m_blendState = nullptr;
	BlendMode m_desiredBlendMode = BlendMode::ALPHA;
	ID3D11BlendState* m_blendStates[(int)(BlendMode::COUNT)] = {};

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	RasterizerMode m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_NONE;
	ID3D11RasterizerState* m_rasterizerStates[(int)RasterizerMode::COUNT] = {};


	ID3D11SamplerState* m_samplerState = nullptr;
	SamplerMode m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	ID3D11SamplerState* m_samplerStates[(int)(SamplerMode::COUNT)] = {};

	ID3D11Texture2D* m_depthStencilTexture = nullptr;
	ID3D11DepthStencilView* m_depthStencilDSV = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	DepthMode m_desiredDepthMode = DepthMode::READ_WRITE_LESS_EQUAL;
	ID3D11DepthStencilState* m_depthStencilStates[(int)(DepthMode::COUNT)] = {};

//-----------------------------------------------------------------------
	
	
	void CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo);

	void BindVertexBuffer(VertexBuffer* vbo);


	void IMGUIStartUp();
	void IMGUIBeginFrame();
	void IMGUIEndFrame();
	void IMGUIShutdown();

	ConstantBuffer* CreateConstantBuffer(const unsigned int size);
	void CopyCPUToGPU(const void* data, unsigned int size, ConstantBuffer* cbo);
	void BindConstantBuffer(int slot, ConstantBuffer* cbo);
	//Blend
	void SetStatesIfChanged();


public:
	
	void SetRasterizerMode(RasterizerMode rasterizerMode);
	void SetDepthMode(DepthMode depthMode);
	void SetBuffers(VertexBuffer* vbo = nullptr, IndexBuffer* ibo = nullptr);

	void SetLightConstants(Vec3 direction, float sunIntensity, float ambientIntensity, Rgba8 const& sunColor=Rgba8::WHITE, std::vector<LightSource> sources = std::vector<LightSource>(0));

	LightSource CreateSpotLight(float intensity, float innerCircle, float outterCircle,Vec3 Position, Rgba8 const& color = Rgba8::WHITE, Vec3 direction = Vec3(1.f,0.f,0.f),
	                 float innerPenumbraDeg = 360.f, float outterPenumbraDeg = 360.f);
	void SetPerFrameConstants(float time, int c_int = 0, float c_float = 0.f);
	
};