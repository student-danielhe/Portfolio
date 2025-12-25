#pragma once
#include "DebugRender.hpp"
#include "Engine/Renderer/Camera.hpp"

class DebugProp {
public:
	DebugProp(DebugRenderMode mode, RasterizerMode rasterMode, float duration, Rgba8 startColor, Rgba8 endColor, std::vector<Vertex_PCU> verts);
	void Render();
	void Update(float deltaSeconds);
	Mat44 GetModelToWorld()const;
	void SetPosition(Vec3 pos);
	void SetTexture(Texture* texture);
	void SetText(std::vector <Vertex_PCU> newVerts);
	void SetBillBoardTransform(const Camera& camera);

	DebugRenderMode m_debugRenderMode;
	float m_duration = 0.f;
	float m_currentDuration = 0.f;
	Rgba8 m_startColor;
	Rgba8 m_endColor;
	RasterizerMode m_rasterizerMode;
	Vec3 m_position;
	Vec3 m_velocity;
	Mat44 m_orientation;
	EulerAngles m_angularVelocity;
	std::vector<Vertex_PCU> m_vertexes;
	Rgba8 m_color = Rgba8(255, 255, 255);
	Texture* m_texture = nullptr;
	bool billboarding = false;
};