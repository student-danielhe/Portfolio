#pragma once
#include "Game.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Player.hpp"
#include "Prop.hpp"
#include "Entity.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Plane.hpp"
#include "Engine/Math/OBB3.hpp"

struct Sphere {
	Vec3* m_center = nullptr;
	float m_radius = 0.f;
	Prop* m_prop = nullptr;
};

struct ZCylinder {
	Vec3* m_bottom = nullptr;
	float m_radius = 0.f;
	FloatRange* m_ZRange = nullptr;
	Prop* m_prop = nullptr;
};

struct Cube {
	AABB3* m_cube = nullptr;
	Prop* m_prop = nullptr;
};

struct TiltedCube {
	OBB3* m_cube = nullptr;
	Prop* m_prop = nullptr;
};

struct Plane {
	Plane3* m_plane = nullptr;
	Prop* m_prop = nullptr;
};

class Game3D :public Game {
public:
	RandomNumberGenerator* g_RNG = nullptr;
	bool m_attractMode = true;
	Camera* g_ScreenCamera = nullptr;
	Clock* m_gameClock = nullptr;
	Player* m_player = nullptr;
	Texture* m_texture = nullptr;
	bool m_stationary = false;
	Vec3 m_stationaryLocation;
	Vec3 m_stationaryForward;

	Prop* m_raycastProp = nullptr;
	bool m_dragging = false;
	Vec3 m_dragVector;
	Cube* m_draggedCube = nullptr;
	Sphere* m_draggedSphere = nullptr;
	ZCylinder* m_draggedCylinder = nullptr;
	EulerAngles m_rotation;

	std::vector<Sphere>      m_sphereList;
	std::vector<ZCylinder> m_cylinderList;
	std::vector<Cube>          m_cubeList;
	std::vector<TiltedCube>    m_OBB3List;
	Plane*                        m_plane3D;

	void StartUp();
	void Update(float deltaSecond);
	void Render();
	void HandleKeyPress();

	
private:
	void CreatePlayerBasis();
	void CreateWorldBasis();
	void CreateRandomSphere(bool wireframe = false);
	void CreatedRandomCube(bool wireframe = false);
	void CreateRandomCylinder(bool wirefram = false);
	void CreateRandomOBB3(bool wireframe = false);
	void CreateRandomPlane();

	bool CheckCubeOverlap(Cube cube);
	bool CheckSphereOverlap(Sphere sphere);
	bool CheckCylinderOverlap(ZCylinder cylinder);
	bool CheckOBB3Overlap(TiltedCube cube);
private:
	Prop* m_playerBasis = nullptr;
	Prop* m_worldBasis = nullptr;
};