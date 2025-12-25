#include "PlayerShip.hpp"
#include<math.h>
#include"Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
//extern Renderer* g_theRenderer;

PlayerShip::PlayerShip(Game* owner, Vec2 const& pos, float ordientationDegree) 
:Entity(owner,pos,ordientationDegree)
{

	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	//halfway
	m_angularVelocity = 0;
	m_verlocity = Vec2(0, 0);
	m_maxHP = 20;
	m_health = 20;
}

void PlayerShip::Update(float deltaSecond) {
	m_position += m_verlocity*deltaSecond;
	m_shootPrep += m_shootRate*deltaSecond*60;
	m_orientationDegrees += m_angularVelocity*deltaSecond;
	if (m_health <= 0) {
		Dies();
		
	}

	HandleKeyPress(deltaSecond);
	Bounce();
}
void PlayerShip::Render() {
	float slow = 1;
	if (m_slowTurn) {
		slow = 0.7f;
	}
	if (m_angularVelocity > 0) {
		RenderTail(-30, slow);
	}if (m_angularVelocity < 0) {
		RenderTail(30, slow);
	}
	Rgba8 color(102,153,204);
	float x = m_position.x;
	float y = m_position.y;
	Vec2 uv(0, 0);
	//Triangle A
	Vec3 A1Pos(x,y,0);
	Vec3 A1Rotation(2 * WORLD_SIZE_X / 400.f, 1 * WORLD_SIZE_X / 400.f, 0);
	A1Pos += A1Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU A1(A1Pos,color,uv);
	Vec3 A2Pos(x, y, 0);
	Vec3 A2Rotation(0, 2 * WORLD_SIZE_X / 400.f, 0);
	A2Pos += A2Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU A2(A2Pos, color, uv);
	Vec3 A3Pos(x , y , 0);
	Vec3 A3Rotation(-2 * WORLD_SIZE_X / 400.f, 1 * WORLD_SIZE_X / 400.f, 0);
	A3Pos += A3Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU A3(A3Pos, color, uv);
	//triangle B
	Vertex_PCU B1 = A3;
	Vec3 B2Pos(x,y,0);
	Vec3 B2Rotation(0, 1 * WORLD_SIZE_X / 400.f, 0);
	B2Pos += B2Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU B2(B2Pos, color, uv);
	Vec3 B3Pos(x , y , 0);
	Vec3 B3Rotation(-2 * WORLD_SIZE_X / 400.f, -1 * WORLD_SIZE_X / 400.f, 0);
	B3Pos += B3Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU B3(B3Pos, color, uv);
	//triangle c
	Vertex_PCU C1 = B2;
	Vertex_PCU C2 = B3;
	Vec3 C3Pos(x, y , 0);
	Vec3 C3Rotation(0, -1 * WORLD_SIZE_X / 400.f, 0);
	C3Pos += C3Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU C3(C3Pos, color, uv);
	//triangle d
	Vertex_PCU D1 = B2;
	Vertex_PCU D2 = C3;
	Vec3 D3Pos(x,y,0);
	Vec3 D3Rotation(1 * WORLD_SIZE_X / 400.f, 0, 0);
	D3Pos += D3Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU D3(D3Pos, color, uv);
	//triangle e
	Vertex_PCU	E1 = B3;
	Vec3 E2Pos(x, y , 0);
	Vec3 E2Rotation(0, -2 * WORLD_SIZE_X / 400.f, 0);
	E2Pos += E2Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU E2(E2Pos, color, uv);
	Vec3 E3Pos(x , y , 0);
	Vec3 E3Rotation(2 * WORLD_SIZE_X / 400.f, -1 * WORLD_SIZE_X / 400.f, 0);
	E3Pos += E3Rotation.GetRotatedAboutZDegrees(m_orientationDegrees);
	Vertex_PCU E3(E3Pos, color, uv);

	Vertex_PCU * arr = new Vertex_PCU[15];
	arr[0] = A1;
	arr[1] = A2;
	arr[2] = A3;
	arr[3] = B1;
	arr[4] = B2;
	arr[5] = B3;
	arr[6] = C1;
	arr[7] = C2;
	arr[8] = C3;
	arr[9] = D1;
	arr[10] = D2;
	arr[11] = D3;
	arr[12] = E1;
	arr[13] = E2;
	arr[14] = E3;
	g_theRenderer->DrawVertexArray(15,arr);
	XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
	//key press check
	if (controller.IsConnected() && controller.GetLeftStick().GetMagnitude() > 0){ 
		RenderTail(0,1); 
	}
	else if (g_theInput->WasKeyJustPressed('E')) {
		if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
			RenderTail(0, 1);
		}
		
	}
	
	
}
void PlayerShip::RenderTail(float deg,float scale) {
	Vertex_PCU* arr = new Vertex_PCU[9];
	Vec2* tailLocal = new Vec2[9];
	//Red part
	Rgba8 red = Rgba8(255, 0, 0);
	arr[0].m_color = red;
	tailLocal[0] = Vec2(-2 * WORLD_SIZE_X / 400.f, 1 * WORLD_SIZE_X / 400.f);
	arr[1].m_color = red;
	tailLocal[1] = Vec2(-2 * WORLD_SIZE_X / 400.f, -1 * WORLD_SIZE_X / 400.f);
	arr[2].m_color = red;
	arr[2].m_color.a = 0;
	RandomNumberGenerator* rand = new RandomNumberGenerator;
	tailLocal[2] = Vec2(rand->RollRandomFloatInRange(-6, -4), 0);
	//Orange
	Rgba8 orange = Rgba8(255, 127, 0);
	arr[3].m_color = orange;
	tailLocal[3] = Vec2(-2.f * WORLD_SIZE_X / 400.f, .1f * WORLD_SIZE_X / 400.f);
	arr[4].m_color = orange;
	tailLocal[4] = Vec2(-2.f * WORLD_SIZE_X / 400.f, -.1f * WORLD_SIZE_X / 400.f);
	arr[5].m_color = orange;
	arr[5].m_color.a = 0;
	tailLocal[5] = Vec2(-4.f * WORLD_SIZE_X / 400.f, 0.f * WORLD_SIZE_X / 400.f);
	//yellow
	Rgba8 yellow = Rgba8(255, 255, 0);
	arr[6].m_color = yellow;
	tailLocal[6] = Vec2(-2.f * WORLD_SIZE_X / 400.f, .5f * WORLD_SIZE_X / 400.f);
	arr[7].m_color = yellow;
	tailLocal[7] = Vec2(-2.f * WORLD_SIZE_X / 400.f, -.5f * WORLD_SIZE_X / 400.f);
	arr[8].m_color = yellow;
	arr[8].m_color.a = 0;
	tailLocal[8] = Vec2(-3.f * WORLD_SIZE_X / 400.f, 0.f * WORLD_SIZE_X / 400.f);

	for (int i = 0; i < 9; i++) {
		tailLocal[i].x *= scale;
		Vec2 temp= m_position+tailLocal[i].GetRotatedDegrees(m_orientationDegrees+deg);
		arr[i].m_position = Vec3(temp.x, temp.y, 0);
		arr[i].m_uvTexCoords = Vec2(0, 0);
	}
	g_theRenderer->DrawVertexArray(9, arr);
}
void PlayerShip::HandleKeyPress(float deltaSecond) {
	if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
		Vec2 accel(PLAYER_SHIP_ACCELERATION, 0);
		XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
		//key press check
		if (controller.IsConnected() && controller.GetLeftStick().GetMagnitude() > 0) {
			Vec2 correctPos = Vec2(controller.GetLeftStick().GetPosition().x, -controller.GetLeftStick().GetPosition().y);
			accel = correctPos / 1000;
			m_verlocity += accel * deltaSecond;
			m_orientationDegrees = accel.GetOrientationDegrees();

		}
		else {
			if (g_theInput->IsKeyJustDown('E')) {
				accel = Vec2(PLAYER_SHIP_ACCELERATION, 0);
				accel.SetOrientationDegrees(m_orientationDegrees);
				m_verlocity += accel * deltaSecond;
			}
			else {
				//friction
				m_verlocity *= 0.95f;
				if (m_verlocity.GetLengthSquared() < 1.0f) {
					m_verlocity = Vec2(0, 0);
				}
			}
			//shift key
			m_slowTurn = g_theInput->IsKeyJustDown(0x10);
			if (g_theInput->IsKeyJustDown('S')) {
				m_angularVelocity = PLAYER_SHIP_TURN_SPEED;
				if (m_slowTurn) {
					m_angularVelocity *= 0.3f;
				}
			}
			else if (g_theInput->IsKeyJustDown('F')) {
				m_angularVelocity = -PLAYER_SHIP_TURN_SPEED;
				if (m_slowTurn) {
					m_angularVelocity *= 0.3f;
				}
			}
			else {
				m_angularVelocity = 0;
			}
		}
	}
}
void PlayerShip::Bounce() {
	//border check
	float xPos = m_position.x;
	float yPos = m_position.y;
	float edge = PLAYER_SHIP_PHYSICS_RADIUS;

	//bounce off wall
	if ((WORLD_SIZE_X - xPos < edge) || (xPos < edge)) {
		m_verlocity.x *= -.5;
	}
	if ((WORLD_SIZE_Y - yPos < edge) || (yPos < edge)) {
		m_verlocity.y *= -.5;
	}
	//bring back in bound
	if (m_position.x < 0 + edge) {
		m_position.x = edge;
	}if (m_position.x > WORLD_SIZE_X - edge) {
		m_position.x = WORLD_SIZE_X - edge;
	}
	if (m_position.y < 0 + edge) {
		m_position.y = edge;
	}if (m_position.y > WORLD_SIZE_Y - edge) {
		m_position.y = WORLD_SIZE_Y - edge;
	}
}
void PlayerShip::KnockedBack(Vec2 const& position) {
	Vec2 normal = (m_position - position).GetNormalized();
	m_verlocity += normal * 20;
}