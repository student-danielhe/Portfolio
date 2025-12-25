#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
class StaticModel {
public:
	Texture* m_diffuse = nullptr;
	Texture* m_normal = nullptr;
	Texture* m_sge = nullptr;
	Shader* m_shader = nullptr;
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;

	Vec3 m_position;
	EulerAngles m_orientation;
	Rgba8 m_color = Rgba8::WHITE;
	StaticModel() {}
};