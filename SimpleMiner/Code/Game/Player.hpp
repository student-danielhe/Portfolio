#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "GameCommon.hpp"
#include "PlayerCharacter.hpp"
enum CameraMode {
	CAMERAMODE_FIRST_PERSON,
	CAMERAMODE_OVER_SHOULDER,
	CAMERAMODE_SPECTATOR_FULL,
	CAMERAMODE_SPECTATOR_XY,
	CAMERAMODE_INDEPENDENT
};
class Player  {
public:
	Game* m_game = nullptr;
	Vec3 m_position = Vec3(0.f, 0.f, 100.f);
	Vec3 m_velocity;
	EulerAngles m_orientation;
	EulerAngles m_angularVelocity;
	std::vector<Vertex_PCU> m_vertexes;
	Rgba8 m_color = Rgba8(255, 255, 255);
	Texture* m_texture = nullptr;
	PlayerCharacter* m_character = nullptr;
	int m_cameraMode;

	Player(Game* owner);
	~Player();
	void Update(float deltaSecond);
	void Render()const;
	Mat44 GetTransform();
	Vec3 GetPosition();
	Vec3 GetFwdVector();
	Vec3 GetOrientation();
	void HandleKeyPressed();
	Vec3 CalculateCameraPosition();
};
