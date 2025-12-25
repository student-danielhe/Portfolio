#include "DebugProp.hpp"
#include "Engine/Core/EngineCommon.hpp"
extern Renderer* g_theRenderer;
void DebugProp::Render() {

	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetRasterizerMode(m_rasterizerMode);
	g_theRenderer->BindTexture(m_texture);
	Mat44 transform;
		transform = GetModelToWorld();
		g_theRenderer->SetModelConstants(transform, m_color);
		if (m_debugRenderMode == DebugRenderMode::ALWAYS) {
			g_theRenderer->SetDepthMode(DepthMode::DISABLED);
		}
		else if (m_debugRenderMode == DebugRenderMode::USE_DEPTH) {
			g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		}

		g_theRenderer->DrawVertexArray(m_vertexes);

		//Second draw for x ray
		if (m_debugRenderMode == DebugRenderMode::X_RAY) {
			g_theRenderer->SetBlendMode(BlendMode::ALPHA);
			g_theRenderer->SetDepthMode(DepthMode::READ_ONLY_ALWAYS);
			g_theRenderer->SetModelConstants(transform, Rgba8::InterpolateColor(m_color, Rgba8(255, 255, 255, 0), .25f));
			g_theRenderer->DrawVertexArray(m_vertexes);
		}
}
void DebugProp::Update(float deltaSeconds) {
	UNUSED(deltaSeconds);
	if (m_duration != -1) {
		float t = (m_currentDuration / m_duration);
		m_color = Rgba8::InterpolateColor(m_startColor, m_endColor, t);
	}
	
	
}
void DebugProp::SetBillBoardTransform(const Camera& camera) {
	if (billboarding) {
		m_orientation = GetBillboardTransform(BillboardType::FULL_OPPPOSING, camera.GetCameraToWorldTransform(), m_position);
		
	}
	
}

Mat44 DebugProp::GetModelToWorld()const {
	Mat44 modelToWOrld = m_orientation;
	return Mat44(modelToWOrld.GetIBasis3D(),modelToWOrld.GetJBasis3D(), modelToWOrld.GetKBasis3D(), m_position);
}
DebugProp::DebugProp(DebugRenderMode mode, RasterizerMode rasterMode, float duration, Rgba8 startColor, Rgba8 endColor, std::vector<Vertex_PCU> verts) {
	m_debugRenderMode = mode;
	m_duration = duration;
	m_startColor = startColor; 
	m_endColor = endColor;
	m_color = startColor;
	m_vertexes = verts;
	m_rasterizerMode = rasterMode;
}

void DebugProp::SetPosition(Vec3 pos) {
	m_position = pos;
}

void DebugProp::SetTexture(Texture* texture) {
	m_texture = texture;
}

void DebugProp::SetText(std::vector<Vertex_PCU> newVerts) {
	m_vertexes.clear();
	m_vertexes = newVerts;
}