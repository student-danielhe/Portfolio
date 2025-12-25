#include "Player.hpp"
Camera* g_thePlayerCamera;
void Player::Update(float deltaSeconds) {
    HandleKeyPressed();
	m_angularVelocity.m_pitchDegrees = 0.f;
	m_angularVelocity.m_rollDegrees = 0.f;
	m_angularVelocity.m_yawDegrees = 0.f;
	m_velocity = Vec3(0.f, 0.f, 0.f);
	float speed = 20.f;
	if (g_theInput->GetCursorMode()==CursorMode::FPS) {
		//rotation------------------------------------------
		Vec2 mouseDelta = g_theInput->GetCursorClientDelta();
		//key press check
		float valX = mouseDelta.x;
		float valY = mouseDelta.y;

		m_orientation.m_yawDegrees -= 0.05f * valX;
		m_orientation.m_pitchDegrees += 0.05f * valY;

		//movement-----------------------------------
		if ((m_cameraMode != CAMERAMODE_FIRST_PERSON && m_cameraMode != CAMERAMODE_OVER_SHOULDER&&m_cameraMode != CAMERAMODE_INDEPENDENT)) {
			if (g_theInput->WasKeyJustPressed('W')) {
				m_velocity.x = speed;
			}
			else if (g_theInput->WasKeyJustPressed('S')) {
				m_velocity.x = -speed;
			}

			if (g_theInput->WasKeyJustPressed('A')) {
				m_velocity.y = speed;
			}
			else if (g_theInput->WasKeyJustPressed('D')) {
				m_velocity.y = -speed;
			}
			if (m_cameraMode == CAMERAMODE_SPECTATOR_FULL) {
			    m_velocity = m_orientation.GetAsMatrix_IFwd_JLeft_KUp().TransformVectorQuantity3D(m_velocity);
			}
			
			if (g_theInput->WasKeyJustPressed('Q')) {
				m_velocity.z = speed;
			}
			if (g_theInput->WasKeyJustPressed('E')) {
				m_velocity.z = -speed;
			}
			if (g_theInput->WasKeyJustPressed(KEYCODE_SHIFT)) {
				m_velocity *= 10.f;
			}
			m_position += m_velocity * deltaSeconds;
			g_thePlayerCamera->SetPosition(m_position);

			
		}
		else {
			if (m_cameraMode != CAMERAMODE_INDEPENDENT) {
				g_thePlayerCamera->SetPosition(CalculateCameraPosition());
				m_position = CalculateCameraPosition();
			}
		}
		

		m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.f, 85.f);

		
		g_thePlayerCamera->SetOrientation(m_orientation);
	}

	if (m_cameraMode == CAMERAMODE_FIRST_PERSON|| m_cameraMode == CAMERAMODE_OVER_SHOULDER||m_cameraMode == CAMERAMODE_INDEPENDENT) {
		if(m_character)
		    m_character->Update(deltaSeconds);
	}
}
void Player::Render()const {
    if (m_cameraMode == CAMERAMODE_FIRST_PERSON|| m_cameraMode == CAMERAMODE_OVER_SHOULDER||m_cameraMode == CAMERAMODE_INDEPENDENT) {
	    if (m_character) {
	    	m_character->Render();
	    }
	}
}
Player::Player(Game* owner) {
    m_game=owner;
	m_position = Vec3(-50.f, -50.f, 80.f);
	m_orientation = EulerAngles(45.f, 45.f, 0.f);
}

Player::~Player()
{
	if (m_character) {
		delete m_character;
		m_character = nullptr;
	}
}

Mat44 Player::GetTransform() {
	Mat44 modelToWorld = Mat44::MakeTranslation3D(m_position);
	modelToWorld.Append(m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
	return modelToWorld;
}

Vec3 Player::GetPosition() {
	return m_position;
}
Vec3 Player::GetFwdVector() {
	return m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D();
}
Vec3 Player::GetOrientation() {
	return Vec3(
	    m_orientation.m_yawDegrees,
		m_orientation.m_pitchDegrees,
		m_orientation.m_rollDegrees);
}

void Player::HandleKeyPressed()
{
	if (g_theInput->WasKeyJustReleased('C')) {
		if (m_cameraMode == CAMERAMODE_INDEPENDENT) {
		    m_cameraMode = CAMERAMODE_FIRST_PERSON;
		}
		else {
		    m_cameraMode = m_cameraMode+1;
		}

	}
}

Vec3 Player::CalculateCameraPosition()
{
	if (m_cameraMode == CAMERAMODE_FIRST_PERSON) {
	    return m_character->m_bottomPosition + Vec3(0.f, 0.f, m_character->m_eyeHeight);
	}
	else if (m_cameraMode == CAMERAMODE_OVER_SHOULDER) {
	    Vec3 eyePos = m_character->m_bottomPosition + Vec3(0.f, 0.f, m_character->m_eyeHeight);
		return eyePos - m_character->m_camDist* GetFwdVector();
	}
	else {
		return m_position;
	}
}
