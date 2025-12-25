#pragma once
#include <vector>
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Controller.hpp"
#include "Weapon.hpp"
#include "Engine/Core/Timer.hpp"
class Game;
enum class CameraMode {
	POSSESS,
	FREEFLY
};
class PlayerController :public Controller{
public:
	PlayerController(ActorHandle* handle, Map* map);
	void UpdateInput(float deltaSecond);
	Vec3 GetFwdVector();
	Vec3 GetOrientation();
	void UpdateCamera();
	void PlayerPossess(ActorHandle* newActor);
	void RenderUI(Weapon* weapon);
	void RenderHUD(Weapon* weapon);
	void RenderWeapon(Weapon* weapon);
	void RenderStats(Weapon* weapon);
	void InitializeCameras();
	void UpdateWeapon();
public:
	Vec3 m_position;
	Vec3 m_velocity;
	EulerAngles m_orientation;
	EulerAngles m_angularVelocity;
	std::vector<Vertex_PCU> m_vertexes;
	Rgba8 m_color = Rgba8(255, 255, 255);
	Texture* m_texture = nullptr;
	CameraMode m_mode = CameraMode::FREEFLY;
	Camera* m_HUDCamera = nullptr;
	Camera* m_worldCamera = nullptr;
	int m_playerIndex = 0;
	float m_aspect = 2.f;
	int m_ammo = 5;
	bool m_aimMeat = false;
	Timer* m_FPSTimer = nullptr;
	std::vector<Vec2*> m_cursorDeltasOver20Frames;
	float m_thirdPersonDist = 2.f;
};
