#include "Prop.hpp"
#include "Game.hpp"
void Prop::Render()const {
	
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->BindTexture(m_texture);
	Mat44 transform = GetModelToWorld();
	g_theRenderer->SetModelConstants(transform, m_color);
	g_theRenderer->DrawVertexArray(m_vertexes);
}
void Prop::Update(float deltaSeconds) {
	EulerAngles rotation = m_angularVelocity;
	rotation.m_pitchDegrees *= deltaSeconds;
	rotation.m_rollDegrees *= deltaSeconds;
	rotation.m_yawDegrees *= deltaSeconds;
	m_orientation.m_pitchDegrees+=rotation.m_pitchDegrees;
	m_orientation.m_rollDegrees += rotation.m_rollDegrees;
	m_orientation.m_yawDegrees += rotation.m_yawDegrees;
}
Mat44 Prop::GetModelToWorld()const {
	Mat44 modelToWOrld = Mat44::MakeTranslation3D(m_position);
	modelToWOrld.Append(m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
	return modelToWOrld;
}
Prop::Prop(Game* owner) :Entity(owner) {
}

void Prop::SetVerts(std::vector<Vertex_PCU> verts) {
	m_vertexes = verts;
}

void Prop::SetAngularVelocity(EulerAngles angular) {
	m_angularVelocity = angular;
}
void Prop::SetVelocity(Vec3 velocity) {
	m_velocity = velocity;
}
void Prop::SetPosition(Vec3 pos) {
	m_position = pos;
}
void Prop::SetAngle(EulerAngles angle) {
	m_orientation = angle;
}
void Prop::SetColor(Rgba8 color) {
	m_color = color;
}
void Prop::SetTexture(Texture* texture) {
	m_texture = texture;
}
