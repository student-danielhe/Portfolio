#include "Game3D.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"

void Game3D::StartUp() {
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(48, 24));

	g_RNG = new RandomNumberGenerator();

	m_player = new Player(this);
	g_thePlayerCamera = new Camera();
	g_thePlayerCamera->SetOrthoView(Vec2(-1, -1), Vec2(1, 1));
	m_gameClock = new Clock();

	CreatePlayerBasis();
	CreateWorldBasis();

	m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");

	CreateRandomCylinder();
	CreateRandomSphere();
	CreatedRandomCube();
	CreateRandomOBB3();

	CreateRandomCylinder();
	CreateRandomSphere();
	CreatedRandomCube();
	CreateRandomOBB3();

	CreateRandomPlane();

	CreateRandomCylinder(true);
	CreateRandomSphere(true);
	CreatedRandomCube(true);
	CreateRandomOBB3(true);
	m_extraText = "WASD: Move, QE: Up/Down, Mouse:Turn, Space: Pause, HoldLMB: Drag, ZXC:Rotate";
	m_textSize = 3.f;
}
void Game3D::Update(float deltaSecond) {
	UNUSED(deltaSecond);
		HandleKeyPress();
		m_player->Update(deltaSecond);
		//float gameDeltaSecond = (float)m_gameClock->GetDeltaSeconds();
		m_playerBasis->SetPosition(m_player->GetPosition() + m_player->GetFwdVector() * 0.2f);

		for (Cube cube : m_cubeList) {
			if (CheckCubeOverlap(cube)) {
				cube.m_prop->SetColor(Rgba8(100, 100, 100));
			}
			else {
				cube.m_prop->SetColor(Rgba8(200, 200, 200));
			}
		}
		for (Sphere sphere : m_sphereList) {
			if (CheckSphereOverlap(sphere)) {
				sphere.m_prop->SetColor(Rgba8(100, 100, 100));
			}
			else {
				sphere.m_prop->SetColor(Rgba8(200, 200, 200));
			}
		}
		for (ZCylinder cylinder : m_cylinderList) {
			if (CheckCylinderOverlap(cylinder)) {
				cylinder.m_prop->SetColor(Rgba8(100, 100, 100));
			}
			else {
				cylinder.m_prop->SetColor(Rgba8(200, 200, 200));
			}
		}
		for (TiltedCube cube : m_OBB3List) {
			if (CheckOBB3Overlap(cube)) {
				cube.m_prop->SetColor(Rgba8(100, 100, 100));
			}
			else {
				cube.m_prop->SetColor(Rgba8(200, 200, 200));
			}
		}
		if (g_theInput->WasKeyJustPressed('Z')) {
			m_rotation.m_yawDegrees = 10.f*deltaSecond;
		}
		else {
			m_rotation.m_yawDegrees = 0.f;
		}
		if (g_theInput->WasKeyJustPressed('X')) {
			m_rotation.m_pitchDegrees = 10.f * deltaSecond;
		}
		else {
			m_rotation.m_pitchDegrees = 0.f;
		}
		if (g_theInput->WasKeyJustPressed('C')) {
			m_rotation.m_rollDegrees = 10.f * deltaSecond;
		}
		else {
			m_rotation.m_rollDegrees = 0.f;
		}

}


void Game3D::Render() {
		//render game mode
		//clear screen
	m_raycastProp = nullptr;
	std::vector<Vec3> nearestPointList;
		Rgba8 darkOrange = Rgba8(50, 50, 50);
		g_theRenderer->ClearScreen(darkOrange);
		g_thePlayerCamera->SetPerspectiveView(2.f, 60.f, .1f, 100.f);
		Mat44 CamToRender(Vec4(0.f, -1.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 1.f, 0.f),
			Vec4(1.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 1.f));
		CamToRender.Transpose();
		g_thePlayerCamera->SetCameraToRenderTransform(CamToRender);
		g_theRenderer->BeginCamera(*g_thePlayerCamera);

		m_playerBasis->Render();
		m_worldBasis->Render();
		float rayDist = 3.f;
		Vec3 rayStart;
		Vec3 fwdNormal;
		if (!m_stationary) {
			rayStart = m_player->GetPosition();
			fwdNormal = m_player->GetFwdVector();
		}
		else {
			rayStart = m_stationaryLocation;
			fwdNormal = m_stationaryForward;
		}
		
		bool hit = false;
		float impactDist;
		
		for (Cube cube : m_cubeList) {
			
			//raycast
			RaycastResult3D result = RaycastVsAABB3D(rayStart, fwdNormal, rayDist, *cube.m_cube);
			if (result.m_didImpact) {
				Prop dot(this);
				std::vector<Vertex_PCU>verts;
				AddVertsForSphere3D(verts, .02f, Rgba8::WHITE);
				dot.SetVerts(verts);
				dot.SetPosition(result.m_impactPos);
				dot.Render();

				Prop temp(this);
				verts.clear();
				AddVertsForArrow3D(verts, Vec3(), result.m_impactNormal, .01f, Rgba8(255, 255, 0));
				temp.SetVerts(verts);
				temp.SetPosition(result.m_impactPos);
				temp.Render();

				cube.m_prop->SetColor(Rgba8::WHITE);
				hit = true;
				impactDist = result.m_impactDist;

				m_raycastProp = cube.m_prop;
				if (m_dragging) {
					Vec3 newPos = cube.m_prop->GetPosition() + m_player->GetVelocity();
					cube.m_prop->SetPosition(newPos);
					cube.m_cube->m_mins += m_player->GetVelocity();
					cube.m_cube->m_maxs += m_player->GetVelocity();
				}
			}

			cube.m_prop->Render();
			//nearest point
			Vec3 nearest = cube.m_cube->GetNearestPoint(rayStart);
			nearestPointList.push_back(nearest);
		}

		for (Sphere sphere : m_sphereList) {
			
			RaycastResult3D result = RaycastVsSphere3D(rayStart, fwdNormal, rayDist, *sphere.m_center,sphere.m_radius);
			if (result.m_didImpact) {
				Prop dot(this);
				std::vector<Vertex_PCU>verts;
				AddVertsForSphere3D(verts, .02f, Rgba8::WHITE);
				dot.SetVerts(verts);
				dot.SetPosition(result.m_impactPos);
				dot.Render();

				Prop temp(this);
				verts.clear();
				AddVertsForArrow3D(verts, Vec3(), result.m_impactNormal, .01f, Rgba8(255, 255, 0));
				temp.SetVerts(verts);
				temp.SetPosition(result.m_impactPos);
				temp.Render();

				sphere.m_prop->SetColor(Rgba8::WHITE);

				m_raycastProp = sphere.m_prop;
				hit = true;
				impactDist = result.m_impactDist;
				if (m_dragging) {
					Vec3 newPos = sphere.m_prop->GetPosition() + m_player->GetVelocity();
					sphere.m_prop->SetPosition(newPos);
					*sphere.m_center += m_player->GetVelocity();
				}
			}

			sphere.m_prop->Render();
			Vec3 nearest = GetNearestPointOnSphere3D(rayStart, *sphere.m_center,sphere.m_radius);
			nearestPointList.push_back(nearest);
		}

		for (ZCylinder cylinder : m_cylinderList) {
			
			RaycastResult3D result = RaycastVsZCylinder3D(rayStart, fwdNormal, rayDist, *cylinder.m_bottom,cylinder.m_radius,*cylinder.m_ZRange);
			if (result.m_didImpact) {
				Prop dot(this);
				std::vector<Vertex_PCU>verts;
				AddVertsForSphere3D(verts, .02f, Rgba8::WHITE);
				dot.SetVerts(verts);
				dot.SetPosition(result.m_impactPos);
				dot.Render();

				Prop temp(this);
				verts.clear();
				AddVertsForArrow3D(verts, Vec3(), result.m_impactNormal, .01f, Rgba8(255, 255, 0));
				temp.SetVerts(verts);
				temp.SetPosition(result.m_impactPos);
				temp.Render();

				cylinder.m_prop->SetColor(Rgba8::WHITE);

				m_raycastProp = cylinder.m_prop;
				hit = true;
				impactDist = result.m_impactDist;
				if (m_dragging) {
					Vec3 newPos = cylinder.m_prop->GetPosition() + m_player->GetVelocity();
					cylinder.m_prop->SetPosition(newPos);
					*cylinder.m_bottom += m_player->GetVelocity();
					cylinder.m_ZRange->m_max += m_player->GetVelocity().z;
					cylinder.m_ZRange->m_min += m_player->GetVelocity().z;
				}
			}
			
			cylinder.m_prop->Render();
			Vec3 nearest = GetNearestPointOnZCylinder3D(rayStart, *cylinder.m_bottom, cylinder.m_radius, *cylinder.m_ZRange);
			nearestPointList.push_back(nearest);
		}

		for (TiltedCube cube : m_OBB3List) {
			RaycastResult3D result = RayCastVSOBB3D(rayStart, fwdNormal, rayDist, *cube.m_cube);
			if (result.m_didImpact) {
				Prop dot(this);
				std::vector<Vertex_PCU>verts;
				AddVertsForSphere3D(verts, .02f, Rgba8::WHITE);
				dot.SetVerts(verts);
				dot.SetPosition(result.m_impactPos);
				dot.Render();

				Prop temp(this);
				verts.clear();
				AddVertsForArrow3D(verts, Vec3(), result.m_impactNormal, .01f, Rgba8(255, 255, 0));
				temp.SetVerts(verts);
				temp.SetPosition(result.m_impactPos);
				temp.Render();

				cube.m_prop->SetColor(Rgba8::WHITE);

				m_raycastProp = cube.m_prop;
				hit = true;
				impactDist = result.m_impactDist;
				if (m_dragging) {
					Vec3 newPos = cube.m_prop->GetPosition() + m_player->GetVelocity();
					cube.m_prop->SetPosition(newPos);
					*cube.m_cube->m_center += m_player->GetVelocity();
					cube.m_cube->m_orientation->m_yawDegrees+=m_rotation.m_yawDegrees;
					cube.m_cube->m_orientation->m_pitchDegrees += m_rotation.m_pitchDegrees;
					cube.m_cube->m_orientation->m_rollDegrees += m_rotation.m_rollDegrees;
					cube.m_prop->SetAngle(*cube.m_cube->m_orientation);

				}
			}

			cube.m_prop->Render();
			Vec3 nearest = GetNearestPointOnOBB3D(rayStart, *cube.m_cube);
			nearestPointList.push_back(nearest);
		}



		m_plane3D->m_prop->Render();
		
		Vec3 nearest = GetNearestPointOnPlane3D(rayStart, *m_plane3D->m_plane);

		nearestPointList.push_back(nearest);

		std::vector<Vertex_PCU>Planeverts;
		RaycastResult3D result = RayCastVSPlane3D(rayStart, fwdNormal, rayDist, *m_plane3D->m_plane);
		AddVertsForArrow3D(Planeverts, Vec3(),m_plane3D->m_plane->m_normal* m_plane3D->m_plane->m_dist, .01f, Rgba8(200, 200, 200));
		Prop temp1(this);
		temp1.SetVerts(Planeverts);
		temp1.Render();

		if (result.m_didImpact) {
			Prop dot(this);
			std::vector<Vertex_PCU>verts;
			AddVertsForSphere3D(verts, .02f, Rgba8::WHITE);
			dot.SetVerts(verts);
			dot.SetPosition(result.m_impactPos);
			dot.Render();

			Prop temp(this);
			verts.clear();
			AddVertsForArrow3D(verts, Vec3(), result.m_impactNormal, .01f, Rgba8(255, 255, 0));
			temp.SetVerts(verts);
			temp.SetPosition(result.m_impactPos);
			temp.Render();
			
			m_plane3D->m_prop->SetColor(Rgba8::WHITE);

			m_raycastProp = m_plane3D->m_prop;
			hit = true;
			impactDist = result.m_impactDist;
			if (m_dragging) {
				Vec3 newPos = m_plane3D->m_prop->GetPosition() + m_player->GetVelocity();
				m_plane3D->m_prop->SetPosition(newPos);
				Vec3 PlanePos = m_plane3D->m_plane->m_normal * m_plane3D->m_plane->m_dist;
				PlanePos += m_player->GetVelocity();
				Vec3 newNormal = PlanePos.GetNormalized();
				float newDist = PlanePos.GetLength();
				m_plane3D->m_plane->m_normal = newNormal;
				m_plane3D->m_plane->m_dist = newDist;
			}
		}

		//render global nearest point
		Vec3 globalNear=Vec3();
		float shortestDistanceSquared = 10000.f;
		for (Vec3 point : nearestPointList) {
			if (GetDistanceSquared3D(point, rayStart) < shortestDistanceSquared) {
				shortestDistanceSquared = GetDistanceSquared3D(point, rayStart);
				globalNear = point;
			}

			Prop dot(this);
			std::vector<Vertex_PCU>verts;
			AddVertsForSphere3D(verts, .1f, Rgba8(255, 100, 0));
			dot.SetVerts(verts);
			dot.SetPosition(point);
			dot.Render();
		}
		Prop dot(this);
		std::vector<Vertex_PCU>verts;
		AddVertsForSphere3D(verts, .12f, Rgba8(0, 255, 0));
		dot.SetVerts(verts);
		dot.SetPosition(globalNear);
		dot.Render();

		//render ray in pause
		if (m_stationary) {
			Prop temp(this);
			verts;
			AddVertsForArrow3D(verts, Vec3(), fwdNormal* rayDist, .01f);
			temp.SetVerts(verts);
			temp.SetPosition(rayStart);
			if (hit) {
				temp.SetColor(Rgba8(150,150,150));
				Prop impact(this);
				std::vector<Vertex_PCU> ImpactVerts;
				AddVertsForArrow3D(ImpactVerts, Vec3(), fwdNormal * impactDist, .011f,Rgba8::RED);
				impact.SetVerts(ImpactVerts);
				impact.SetPosition(rayStart);
				impact.Render();

			}
			else {
				temp.SetColor(Rgba8::GREEN);
			}
			temp.Render();
		}
		g_theRenderer->EndCamera(*g_thePlayerCamera);



	//Render screen
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);
	g_theRenderer->EndCamera(*g_ScreenCamera);


}

void Game3D::HandleKeyPress() {
	if (g_theInput->WasKeyJustReleased(' ')) {
		if (!m_stationary) {
			m_stationary = true;
			m_stationaryLocation = m_player->GetPosition();
			m_stationaryForward = m_player->GetFwdVector();
		}
		else {
			m_stationary = false;
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
		if (m_raycastProp) {
			m_dragging = true;
		}
	}
	if (g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE)) {
		if (m_dragging) {
			m_dragging = false;
			m_draggedCube = nullptr;
			m_draggedCylinder = nullptr;
			m_draggedSphere = nullptr;
		}
	}
}

void Game3D::CreatePlayerBasis() {
	std::vector<Vertex_PCU> verts;
	verts.reserve(1000);
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(.01f, 0.f, 0.f), .0005f, Rgba8(255, 0, 0),10);
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(0.f, .01f, 0.f), .0005f, Rgba8(0, 255, 0),10);
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, .01f), .0005f, Rgba8(0, 0, 255),10);

	m_playerBasis = new Prop(this);
	m_playerBasis->SetVerts(verts);
}

void Game3D::CreateWorldBasis() {
	std::vector<Vertex_PCU> verts;
	verts.reserve(1000);
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(1.f, 0.f, 0.f), .05f, Rgba8(255, 0, 0),10);
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f), .05f, Rgba8(0, 255, 0),10);
	AddVertsForArrow3D(verts, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), .05f, Rgba8(0, 0, 255),10);

	m_worldBasis = new Prop(this);
	m_worldBasis->SetVerts(verts);
}

void Game3D::CreateRandomSphere(bool wireframe) {
	int slices = g_RNG->RollRandomIntInRange(6, 24);
	int stacks = g_RNG->RollRandomIntInRange(4, 8);
	float radius = g_RNG->RollRandomFloatInRange(1.f, 2.f);
	Vec3* center = new Vec3(g_RNG->RollRandomFloatInRange(-10.f,10.f), g_RNG->RollRandomFloatInRange(-10.f, 10.f), g_RNG->RollRandomFloatInRange(-10.f, 10.f));
	Sphere newSphere;
	newSphere.m_center = center;
	newSphere.m_radius = radius;
	Prop* prop= new Prop(this);
	std::vector<Vertex_PCU> verts;
	AddVertsForSphere3D(verts, radius, Rgba8::WHITE, AABB2::DEFAULT, slices, stacks);
	prop->SetVerts(verts);
	if(!wireframe)
	    prop->SetTexture(m_texture);
	prop->SetPosition(*center);
	prop->m_wireframe = wireframe;
	newSphere.m_prop = prop;
	m_sphereList.push_back(newSphere);
}

void Game3D::CreatedRandomCube(bool wireframe) {
	Vec3* center = new Vec3(g_RNG->RollRandomFloatInRange(-10.f, 10.f), g_RNG->RollRandomFloatInRange(-10.f, 10.f), g_RNG->RollRandomFloatInRange(-10.f, 10.f));
	Cube newCube;
	AABB3* cube = new AABB3(Vec3(0.f,0.f,0.f), 
		Vec3(g_RNG->RollRandomFloatInRange(1.f,2.f), g_RNG->RollRandomFloatInRange(1.f, 2.f), g_RNG->RollRandomFloatInRange(1.f, 2.f)));
	Vec3 mins = *center + cube->m_mins;
	Vec3 maxs = *center + cube->m_maxs;
	newCube.m_cube = new AABB3(mins, maxs);
	std::vector<Vertex_PCU> verts;
	AddVertsForAABB3D(verts,*cube);
	Prop* prop=new Prop(this);
	prop->SetVerts(verts);
	if(!wireframe)
	    prop->SetTexture(m_texture);
	prop->SetPosition(*center);
	prop->m_wireframe = wireframe;
	newCube.m_prop = prop;
	m_cubeList.push_back(newCube);

}

void Game3D::CreateRandomCylinder(bool wirefram) {
	Vec3* center = new Vec3(g_RNG->RollRandomFloatInRange(-10.f, 10.f), g_RNG->RollRandomFloatInRange(-10.f, 10.f), g_RNG->RollRandomFloatInRange(-10.f, 10.f));
	float radius = g_RNG->RollRandomFloatInRange(.5f, 1.f);
	float height = g_RNG->RollRandomFloatInRange(1.f, 2.f);
	int slices = g_RNG->RollRandomIntInRange(6, 24);
	FloatRange* ZRange = new FloatRange(center->z, center->z + height);
	ZCylinder cylinder;
	cylinder.m_bottom = center;
	cylinder.m_radius = radius;
	cylinder.m_ZRange = ZRange;

	std::vector<Vertex_PCU> verts;
	AddVertsForZCylinder3D(verts, Vec3(0.f, 0.f, 0.f), height, radius, Rgba8::WHITE, AABB2::DEFAULT, slices);
	Prop* prop= new Prop(this);
	prop->SetVerts(verts);
	if(!wirefram)
	    prop->SetTexture(m_texture);
	prop->SetPosition(*center);
	prop->m_wireframe = wirefram;
	cylinder.m_prop = prop;
	m_cylinderList.push_back(cylinder);
}

void Game3D::CreateRandomOBB3(bool wireframe) {
	Vec3* center = new Vec3(g_RNG->RollRandomFloatInRange(-10.f, 10.f), g_RNG->RollRandomFloatInRange(-10.f, 10.f), g_RNG->RollRandomFloatInRange(-10.f, 10.f));
	Vec3* dimension = new Vec3(g_RNG->RollRandomFloatInRange(0.f, 2.f), g_RNG->RollRandomFloatInRange(0.f, 2.f), g_RNG->RollRandomFloatInRange(0.f, 2.f));
	EulerAngles* orientation = new EulerAngles(g_RNG->RollRandomFloatInRange(0.f, 360.f), g_RNG->RollRandomFloatInRange(0.f, 360.f), g_RNG->RollRandomFloatInRange(0.f, 360.f));
	TiltedCube newCube;
	OBB3* cube = new OBB3(center,dimension, orientation);
	newCube.m_cube = cube;
	std::vector<Vertex_PCU> verts;
	AddVertsForAABB3D(verts, cube->GetAsLocalAABB3());
	Prop* prop = new Prop(this);
	prop->SetVerts(verts);
	if (!wireframe)
		prop->SetTexture(m_texture);
	prop->SetPosition(*center);
	prop->SetAngle(*cube->m_orientation);
	prop->m_wireframe = wireframe;
	newCube.m_prop = prop;
	m_OBB3List.push_back(newCube);

}

void Game3D::CreateRandomPlane() {
	float x = g_RNG->RollRandomFloatInRange(-1.f, 1.f);
	float y = g_RNG->RollRandomFloatInRange(-1.f, 1.f);
	float z = g_RNG->RollRandomFloatInRange(-1.f, 1.f);

	Vec3 normal = Vec3(x,y,z).GetNormalized();
	float dist = g_RNG->RollRandomFloatInRange(0.f, 5.f);
	Plane3 * plane = new Plane3();
	plane->m_normal = normal;
	plane->m_dist = dist;
	m_plane3D = new Plane();
	std::vector<Vertex_PCU> verts;
	AddVertsForPlane3D(verts, *plane);
	Prop* prop = new Prop(this);
	prop->SetVerts(verts);
	prop->SetPosition(Vec3());
	m_plane3D->m_plane = plane;
	m_plane3D->m_prop = prop;
}

bool Game3D::CheckCubeOverlap(Cube cube) {
	for (Cube other : m_cubeList) {
		if (other.m_prop != cube.m_prop) {
			if (DoAABB3sOverlap(*other.m_cube, *cube.m_cube)) {
				return true;
			}
		}
	}
	for (Sphere other : m_sphereList) {
		if (DoSphereOverlapAABB3(*other.m_center, other.m_radius, *cube.m_cube)) {
			return true;
		}
	}
	for (ZCylinder other : m_cylinderList) {
		if (DoAABB3OverlapZCylinder(*cube.m_cube, *other.m_bottom, other.m_radius, *other.m_ZRange)) {
			return true;
		}
	}
	if (DoAABB3OverlapPlane3D(*cube.m_cube, *m_plane3D->m_plane)) {
		return true;
	}
	return false;
}

bool Game3D::CheckSphereOverlap(Sphere sphere) {
	for (Cube other : m_cubeList) {
		if (DoSphereOverlapAABB3(*sphere.m_center, sphere.m_radius, *other.m_cube)) {
			return true;
		}
	}
	for (Sphere other : m_sphereList) {
		if (other.m_prop != sphere.m_prop) {
			if (DoSpheresOverlap(*other.m_center,other.m_radius,*sphere.m_center,sphere.m_radius)) {
				return true;
			}
		}
	}
	for (ZCylinder other : m_cylinderList) {
		if (DoSphereOverlapZCylinder(*sphere.m_center, sphere.m_radius, *other.m_bottom, other.m_radius, *other.m_ZRange)) {
			return true;
		}
	}
	for (TiltedCube other : m_OBB3List) {
		if (DoOBB3OverlapSphere3D(*sphere.m_center, sphere.m_radius, *other.m_cube)) {
			return true;
		}
	}
	if (DoSphereOverlapPlane3D(*sphere.m_center, sphere.m_radius, *m_plane3D->m_plane)) {
		return true;
	}
	return false;
}

bool Game3D::CheckCylinderOverlap(ZCylinder cylinder) {
	for (Cube other : m_cubeList) {
		if (DoAABB3OverlapZCylinder(*other.m_cube, *cylinder.m_bottom, cylinder.m_radius, *cylinder.m_ZRange)) {
			return true;
		}
	}
	for (Sphere other : m_sphereList) {
		if (DoSphereOverlapZCylinder(*other.m_center, other.m_radius, *cylinder.m_bottom, cylinder.m_radius, *cylinder.m_ZRange)) {
			return true;
		}
	}
	for (ZCylinder other : m_cylinderList) {
		if (other.m_prop != cylinder.m_prop) {
			if (DoZCylindersOverlap(*other.m_bottom, other.m_radius, *other.m_ZRange, *cylinder.m_bottom, cylinder.m_radius, *cylinder.m_ZRange)) {
				return true;
			}
		}
	}
	return false;
}

bool Game3D::CheckOBB3Overlap(TiltedCube cube) {
	for (Sphere other : m_sphereList) {
		if (DoOBB3OverlapSphere3D(*other.m_center, other.m_radius, *cube.m_cube)) {
			return true;
		}
	}
	if (DoOBB3OverlapPlane3D(*cube.m_cube, *m_plane3D->m_plane)) {
		return true;
	}
	return false;
}