#pragma once
#include <vector>
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "GameCommon.hpp"
class Game;
class Entity {
public:
	Entity(Game* owner);
	virtual ~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;


public:
	Game* m_game = nullptr;
	Vec3 m_position;
	Vec3 m_velocity;
	EulerAngles m_orientation;
	EulerAngles m_angularVelocity;
	std::vector<Vertex_PCU> m_vertexes;
	Rgba8 m_color=Rgba8(255,255,255);
	Texture* m_texture = nullptr;
};