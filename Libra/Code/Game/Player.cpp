#include"Player.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include"Game.hpp"
#include "Map.hpp"
#include "Engine/Core/EngineCommon.hpp"
void Player::Update(float deltaSecond) {
	if (m_health <= 0) {
		m_isDead = true;
	}
	if (!m_isDead) {
		HandleKeyPress(deltaSecond);
		m_position += m_verlocity * deltaSecond;
		if (m_currentFireCooldown > 0) {

			m_currentFireCooldown -= deltaSecond;
		}
	}
	
}
void Player::Render(std::vector<Vertex_PCU>& verts) {
	UNUSED(verts);
	if (!m_isDead) {
		Texture* tank = g_theRenderer->CreateOrGetTextureFromFile("Images/PlayerTankBase.png");
		g_theRenderer->BindTexture(tank);
		std::vector<Vertex_PCU>arr;
		GlobalVertexArray(arr);
		std::vector<Vertex_PCU>ToRender;
		AddVertsForRect(ToRender, arr[0], arr[1], arr[2], arr[3]);


		g_theRenderer->DrawVertexArray(ToRender);

		Texture* turret = g_theRenderer->CreateOrGetTextureFromFile("Images/PlayerTankTop.png");
		g_theRenderer->BindTexture(turret);
		std::vector<Vertex_PCU> turretArr;
		TurretGlobalVertexArray(turretArr);
		std::vector<Vertex_PCU>TurretToRender;
		AddVertsForRect(TurretToRender, turretArr[0], turretArr[1], turretArr[2], turretArr[3]);
		g_theRenderer->DrawVertexArray(TurretToRender);
		if (g_theGame->m_debugLine) {
			g_theRenderer->BindTexture(nullptr);
			DebugDrawLine(m_position + Vec2(m_cosmeticRadius, 0).GetRotatedDegrees(m_targetAngle), m_position + Vec2(m_physicsRadius * 2, 0).GetRotatedDegrees(m_targetAngle), 3, Rgba8(150, 0, 150));
			DebugDrawLine(m_position, m_position + Vec2(m_physicsRadius * 2, 0).GetRotatedDegrees(m_orientationDegrees + m_turretOrientationDegrees), 3, Rgba8(150, 0, 150));
		}
	}
	
}
void Player::HandleKeyPress(float deltaSecond) {
	Vec2 targetOrientation=Vec2(0,0);
	XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
	//key press check
	if (controller.IsConnected() && controller.GetLeftStick().GetMagnitude() > 0) {
		Vec2 correctPos = Vec2(controller.GetLeftStick().GetPosition().x, -controller.GetLeftStick().GetPosition().y);
		targetOrientation = correctPos;
	}
	else {
		if (g_theInput->IsKeyJustDown('W')) {
			targetOrientation += Vec2(0,1);
		}
		else if (g_theInput->IsKeyJustDown('S')) {
			targetOrientation += Vec2(0, -1);
		}
		else if (g_theInput->IsKeyJustDown('A')) {
			targetOrientation += Vec2(-1, 0);
		}
		else if (g_theInput->IsKeyJustDown('D')) {
			targetOrientation += Vec2(1,0);
		}
		if (g_theInput->IsKeyJustDown(' ')) {
			Shoot(deltaSecond);
		}
	}
	if (targetOrientation != Vec2(0,0)) {
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, targetOrientation.GetOrientationDegrees(), g_gameConfigBlackboard.GetValue("PLAYER_TURN_SPEED", 180.f) *deltaSecond);
		m_verlocity = Vec2(g_gameConfigBlackboard.GetValue("PLAYER_SPEED", 1.f), 0).GetRotatedDegrees(m_orientationDegrees);
	}
	else {
		m_verlocity = Vec2(0, 0);
	}
	targetOrientation = Vec2(0, 0);
	//key press check
	if (controller.IsConnected() && controller.GetRightStick().GetMagnitude() > 0) {
		Vec2 correctPos = Vec2(controller.GetRightStick().GetPosition().x, -controller.GetRightStick().GetPosition().y);
		targetOrientation = correctPos;
	}
	else {
		if (g_theInput->IsKeyJustDown(0x26)) {
			targetOrientation += Vec2(0, 1);
		}
		else if (g_theInput->IsKeyJustDown(0x28)) {
			targetOrientation += Vec2(0, -1);
		}
		else if (g_theInput->IsKeyJustDown(0x25)) {
			targetOrientation += Vec2(-1, 0);
		}
		else if (g_theInput->IsKeyJustDown(0x27)) {
			targetOrientation += Vec2(1, 0);
		}
	}
	if (targetOrientation != Vec2(0, 0)) {
		m_turretOrientationDegrees = GetTurnedTowardDegrees(m_turretOrientationDegrees, targetOrientation.GetOrientationDegrees()-m_orientationDegrees, g_gameConfigBlackboard.GetValue("TURRET_TURN_SPEED", 360.f) * deltaSecond);
	}
	m_targetAngle = targetOrientation.GetOrientationDegrees();
}
Player::Player(Map* owner, Vec2 const& pos, float ordientationDegree)
	:Entity(owner, pos, ordientationDegree)
{
	m_health = g_gameConfigBlackboard.GetValue("PLAYER_MAX_HP", 30);
	m_orientationDegrees = 90;
	m_physicsRadius = g_gameConfigBlackboard.GetValue("PLAYER_PHYSICS_RADIUS", .5f);
	m_cosmeticRadius = g_gameConfigBlackboard.GetValue("PLAYER_COSMETIC_RADIUS", .7f);
	m_turretOrientationDegrees = m_orientationDegrees;
	//
	m_localVertexArray = std::vector<Vertex_PCU>(4, Vertex_PCU());
	m_localVertexArray[0].m_position = Vec3(-m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_localVertexArray[0].m_uvTexCoords = Vec2(0, 0);
	m_localVertexArray[1].m_position = Vec3(m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_localVertexArray[1].m_uvTexCoords = Vec2(1, 0);
	m_localVertexArray[2].m_position = Vec3(m_cosmeticRadius, m_cosmeticRadius, 0);
	m_localVertexArray[2].m_uvTexCoords = Vec2(1, 1);
	m_localVertexArray[3].m_position = Vec3(-m_cosmeticRadius, m_cosmeticRadius, 0);
	m_localVertexArray[3].m_uvTexCoords = Vec2(0, 1);
	//turret
	m_turretLocalVertexArray = std::vector<Vertex_PCU>(4, Vertex_PCU());
	m_turretLocalVertexArray[0].m_position = Vec3(-m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_turretLocalVertexArray[0].m_uvTexCoords = Vec2(0, 0);
	m_turretLocalVertexArray[1].m_position = Vec3(m_cosmeticRadius, -m_cosmeticRadius, 0);
	m_turretLocalVertexArray[1].m_uvTexCoords = Vec2(1, 0);
	m_turretLocalVertexArray[2].m_position = Vec3(m_cosmeticRadius, m_cosmeticRadius, 0);
	m_turretLocalVertexArray[2].m_uvTexCoords = Vec2(1, 1);
	m_turretLocalVertexArray[3].m_position = Vec3(-m_cosmeticRadius, m_cosmeticRadius, 0);
	m_turretLocalVertexArray[3].m_uvTexCoords = Vec2(0, 1);


}

void Player::TurretGlobalVertexArray(std::vector<Vertex_PCU>& result) {
	;
	for (Vertex_PCU vertex : m_turretLocalVertexArray) {
		Vec2 position(vertex.m_position.x, vertex.m_position.y);
		position = position.GetRotatedDegrees(m_orientationDegrees+m_turretOrientationDegrees);
		position += m_position;
		Vertex_PCU globalVertex;
		globalVertex.m_position = Vec3(position.x, position.y, 0);
		globalVertex.m_color = Rgba8(255, 255, 255);
		globalVertex.m_uvTexCoords = vertex.m_uvTexCoords;
		result.push_back(globalVertex);
	}
}
void Player::Shoot(float deltaSecond) {
	UNUSED(deltaSecond);
	if (m_currentFireCooldown > 0) {
		
	}
	else {
		m_currentFireCooldown = g_gameConfigBlackboard.GetValue("PLAYER_FIRE_DELAY", .1f);
		m_owner->SpawnNewEntity(ENTITYTYPE_GOOD_BULLET, FACTION_GOOD, m_position+Vec2(m_physicsRadius,0).GetRotatedDegrees(m_orientationDegrees + m_turretOrientationDegrees), m_orientationDegrees + m_turretOrientationDegrees);
	}
}