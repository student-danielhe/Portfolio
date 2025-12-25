#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>
enum class DebugRenderMode {
	ALWAYS,
	USE_DEPTH,
	X_RAY,
};
struct DebugRenderConfig {
	Renderer* m_renderer = nullptr;
	std::string m_fontPath = "Data/Fonts/";
	std::string m_fontName = "SquirrelFixedFont";
};

void DebugRenderSystemStartUp(const DebugRenderConfig& config);
void DebugRenderSystemShutdown();

void DebugRenderSetVisible();
void DebugRenderSetHidden();
void DebugRenderClear();

void DebugRenderBeginFrame();
void DebugRenderWorld(const Camera& camera);
void DebugRenderScreen(const Camera& camera);
void DebugRenderEndFrame();

void DebugAddWorldSphere(const Vec3& center, float radius, float duration, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255), DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldWireSphere(const Vec3& center, float radius, float duration, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255), DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldCylinder(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255), DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldWireCylinder(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255), DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldArrow(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255), DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldWireArrow(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255), DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddBasis(const Mat44& transform, float radius, float length, float duration, float colorScale = 1.f, float alphaScale = 1.f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldBasis(const Mat44& transform, float duration, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldText(const std::string& text, const Mat44& transform, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255), DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldBillboardText(const std::string& text, const Vec3& origin, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255), DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

void DebugAddScreenText(const std::string& text, const AABB2& box, float cellHeight, const Vec2& alignment, float durationconst, const Rgba8& startColor = Rgba8(255, 255, 255), const Rgba8& endColor = Rgba8(255, 255, 255));
void DebugAddMessage(const std::string& text, const Rgba8& color = Rgba8(255, 255, 255));

bool Command_DebugRenderClear(EventArgs& args);
bool Command_DebugRenderToggle(EventArgs& args);