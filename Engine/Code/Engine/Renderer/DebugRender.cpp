#include "DebugRender.hpp"
#include "DebugProp.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include <mutex>
std::mutex g_debugRenderMutex;
DebugRenderConfig m_debugRenderConfig;
bool m_debugRenderVisible = true;
extern Renderer* g_theRenderer;
std::vector<DebugProp*> m_debugRenderProps;
std::vector<DebugProp*> m_debugRenderScreenProps;
DebugProp* m_timeText = nullptr;
DebugProp* m_posText = nullptr;
BitmapFont* m_debugFont = nullptr;
void DebugRenderSystemStartUp(const DebugRenderConfig& config) {
	m_debugRenderConfig = config;
	std::string textureName = m_debugRenderConfig.m_fontPath;
	textureName.append(m_debugRenderConfig.m_fontName.c_str());
	m_debugFont = g_theRenderer->CreateOrGetBitmapFont(textureName.c_str());
}
void DebugRenderSystemShutdown() {
	DebugRenderClear();
}

void DebugRenderSetVisible() {
	m_debugRenderVisible = true;
}
void DebugRenderSetHidden() {
	m_debugRenderVisible = false;
}
void DebugRenderClear() {
	m_debugRenderProps.clear();
}

void DebugRenderBeginFrame() {

}
void DebugRenderWorld(const Camera& camera) {
	for (DebugProp* prop : m_debugRenderProps) {
		prop->SetBillBoardTransform(camera);
	}
	if (m_debugRenderVisible) {
		m_debugRenderConfig.m_renderer->BeginCamera(camera);
		for (DebugProp* object : m_debugRenderProps) {
			object->Render();
		}
		m_debugRenderConfig.m_renderer->EndCamera(camera);
	}

}
void DebugRenderScreen(const Camera& camera) {
	if (m_debugRenderVisible) {
		m_debugRenderConfig.m_renderer->BeginCamera(camera);
		for (DebugProp* object : m_debugRenderScreenProps) {
			object->Render();
		}
		m_debugRenderConfig.m_renderer->EndCamera(camera);
	}
}
void DebugRenderEndFrame() {

}

void DebugAddWorldSphere(const Vec3& center, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode) {
    g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;
	AddVertsForSphere3D(verts, radius);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::SOLID_CULL_BACK, duration, startColor, endColor, verts);
	prop->SetPosition(center);
	m_debugRenderProps.push_back(prop);
	g_debugRenderMutex.unlock();
}
void DebugAddWorldWireSphere(const Vec3& center, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode) {
    g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;
	AddVertsForSphere3D(verts, radius);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::WIREFRAME_CULL_NONE, duration, startColor, endColor, verts);
	prop->SetPosition(center);
	m_debugRenderProps.push_back(prop);
	g_debugRenderMutex.unlock();
}
void DebugAddWorldCylinder(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode) {
    g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;
	AddVertsForCylinder3D(verts, start, end, radius);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::SOLID_CULL_BACK, duration, startColor, endColor, verts);
	m_debugRenderProps.push_back(prop);
	g_debugRenderMutex.unlock();
}
void DebugAddWorldWireCylinder(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode) {
g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;
	AddVertsForCylinder3D(verts, start, end, radius);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::WIREFRAME_CULL_NONE, duration, startColor, endColor, verts);
	m_debugRenderProps.push_back(prop);
	g_debugRenderMutex.unlock();
}
void DebugAddWorldArrow(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode) {
g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;
	AddVertsForArrow3D(verts, start, end, radius);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::SOLID_CULL_BACK, duration, startColor, endColor, verts);
	m_debugRenderProps.push_back(prop);
	g_debugRenderMutex.unlock();
}
void DebugAddWorldWireArrow(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode) {
g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;
	AddVertsForArrow3D(verts, start, end, radius);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::WIREFRAME_CULL_NONE, duration, startColor, endColor, verts);
	m_debugRenderProps.push_back(prop);
	g_debugRenderMutex.unlock();
}
void DebugAddBasis(const Mat44& transform, float radius, float length, float duration, float colorScale, float alphaScale, DebugRenderMode mode) {
g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(length, 0.f, 0.f), radius, Rgba8((unsigned char)(255 * colorScale), 0, 0, (unsigned char)(255 * alphaScale)));
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(0.f, length, 0.f), radius, Rgba8(0, (unsigned char)(255 * colorScale), 0, (unsigned char)(255 * alphaScale)));
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, length), radius, Rgba8(0, 0, (unsigned char)(255 * colorScale), (unsigned char)(255 * alphaScale)));
	TransformVertexArray3D(verts, transform);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::SOLID_CULL_BACK, duration, Rgba8(255, 255, 255), Rgba8(255, 255, 255), verts);
	m_debugRenderProps.push_back(prop);
	g_debugRenderMutex.unlock();
}
void DebugAddWorldBasis(const Mat44& transform, float duration, DebugRenderMode mode) {
	DebugAddBasis(transform, .3f, 5.f, duration, 1.f, 1.f, mode);
}
void DebugAddWorldText(const std::string& text, const Mat44& transform, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode) {
	g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;

	m_debugFont->AddVertsForText3DAtOriginXForward(verts, textHeight, text, startColor, 1.f, alignment);
	TransformVertexArray3D(verts, transform);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::SOLID_CULL_NONE, duration, startColor, endColor, verts);
	prop->SetTexture(&m_debugFont->GetTexture());
	m_debugRenderProps.push_back(prop);
	g_debugRenderMutex.unlock();
}
void DebugAddWorldBillboardText(const std::string& text, const Vec3& origin, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode) {
	g_debugRenderMutex.lock();
	std::vector<Vertex_PCU> verts;
	m_debugFont->AddVertsForText3DAtOriginXForward(verts, textHeight, text, startColor, 1.f, alignment);
	DebugProp* prop = new DebugProp(mode, RasterizerMode::SOLID_CULL_NONE, duration, startColor, endColor, verts);
	prop->SetTexture(&m_debugFont->GetTexture());
	m_debugRenderProps.push_back(prop);
	prop->m_position = origin;
	prop->billboarding = true;
	g_debugRenderMutex.unlock();
}

void DebugAddScreenText(const std::string& text, const AABB2& box, float cellHeight, const Vec2& alignment, float duration, const Rgba8& startColor, const Rgba8& endColor) {
	UNUSED(text);
	UNUSED(box);
	UNUSED(cellHeight);
	UNUSED(alignment);
	UNUSED(duration);
	UNUSED(startColor);
	UNUSED(endColor);
	//Don't use! Causing memory leaks
	/*std::vector<Vertex_PCU> verts;
	m_debugFont->AddVertsForTextInBox2D(verts, text, box, cellHeight, startColor, 1.f, alignment);
	DebugProp* prop = new DebugProp(DebugRenderMode::ALWAYS, RasterizerMode::SOLID_CULL_NONE, duration, startColor, endColor, verts);
	prop->SetTexture(&m_debugFont->GetTexture());
	m_debugRenderScreenProps.push_back(prop);*/

}

void DebugAddMessage(const std::string& text, const Rgba8& color) {
	DebugAddScreenText(text, AABB2(Vec2(0.f, 0.f), Vec2(100.f, 100.f)), 1.f, Vec2(0.f, 0.f), -1.f, color, color);
}

bool Command_DebugRenderClear(EventArgs& args) {
	UNUSED(args);
	DebugRenderClear();
	return true;
}
bool Command_DebugRenderToggle(EventArgs& args) {
	UNUSED(args);
	m_debugRenderVisible = !m_debugRenderVisible;
	return true;
}