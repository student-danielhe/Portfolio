#include "Player.hpp"
Camera* g_thePlayerCamera;
void Player::Update(float deltaSeconds) {
	m_angularVelocity.m_pitchDegrees = 0.f;
	m_angularVelocity.m_rollDegrees = 0.f;
	m_angularVelocity.m_yawDegrees = 0.f;
	m_velocity = Vec3(0.f, 0.f, 0.f);
	float speed = 2.f;
	if (g_theInput->GetCursorMode()==CursorMode::FPS) {
		//rotation------------------------------------------
		Vec2 mouseDelta = g_theInput->GetCursorClientDelta();
		XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
		//key press check
		float valX = mouseDelta.x;
		float valY = mouseDelta.y;
		//rewrite movement if controller exist
		if (controller.IsConnected() && controller.GetRightStick().GetPosition().x != 0.f) {
			valX = controller.GetRightStick().GetPosition().x;
		}
		if (controller.IsConnected() && controller.GetRightStick().GetPosition().y != 0.f) {
			valY = controller.GetRightStick().GetPosition().y;
		}
		m_orientation.m_yawDegrees -= 0.05f * valX;
		m_orientation.m_pitchDegrees += 0.05f * valY;
		if (controller.IsConnected() && controller.GetLeftTrigger()) {
			m_orientation.m_rollDegrees += controller.GetLeftTrigger() * deltaSeconds;
		}
		else if (controller.IsConnected() && controller.GetRightTrigger()) {
			m_orientation.m_rollDegrees -= controller.GetRightTrigger() * deltaSeconds;
		}
		else if (g_theInput->WasKeyJustPressed('Q')) {
			m_orientation.m_rollDegrees +=90.f*deltaSeconds;
		}
		else if(g_theInput->WasKeyJustPressed('E')) {
			m_orientation.m_rollDegrees += -90.f*deltaSeconds;
		}
		float controllerSpeed = 0.01f;
		//movement-----------------------------------
		if (controller.IsConnected() && controller.GetRightStick().GetPosition().y != 0.f) {
			m_velocity.x = controller.GetRightStick().GetPosition().y*controllerSpeed;
		}
		else if (g_theInput->WasKeyJustPressed('W')) {
			m_velocity.x = speed;
		}
		else if (g_theInput->WasKeyJustPressed('S')) {
			m_velocity.x = -speed;
		}

		if (controller.IsConnected() && controller.GetRightStick().GetPosition().x != 0.f) {
			m_velocity.y = controller.GetRightStick().GetPosition().x*controllerSpeed;
		}
		else if (g_theInput->WasKeyJustPressed('A')) {
			m_velocity.y = speed;
		}
		else if (g_theInput->WasKeyJustPressed('D')) {
			m_velocity.y = -speed;
		}
		
		m_velocity = m_orientation.GetAsMatrix_IFwd_JLeft_KUp().TransformVectorQuantity3D(m_velocity);
		if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_LB)) {
			m_velocity.z = speed;
		}
		else if (g_theInput->WasKeyJustPressed('Z')) {
			m_velocity.z = speed;
		}
		if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_RB)) {
			m_velocity.z = -speed;
		}
		else if (g_theInput->WasKeyJustPressed('C')) {
			m_velocity.z = -speed;
		}
		if (controller.IsConnected() && controller.WasButtonPressed(XBOX_BUTTON_A)) {
			m_velocity *= 10.f;
		}
		else if (g_theInput->WasKeyJustPressed(KEYCODE_SHIFT)) {
			m_velocity *= 10.f;
		}
		m_position += m_velocity * deltaSeconds;
		
		if (controller.IsConnected() && controller.WasButtonReleased(XBOX_BUTTON_START)) {
			m_position = Vec3(0.f, 0.f, 2.f);
			m_orientation = EulerAngles(0.f, 0.f, 0.f);
		}
		else if (g_theInput->WasKeyJustReleased('H')) {
			m_position = Vec3(0.f, 0.f, 2.f);
			m_orientation = EulerAngles(0.f, 0.f, 0.f);
		}

		m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.f, 85.f);
		m_orientation.m_rollDegrees = GetClamped(m_orientation.m_rollDegrees, -45.f, 45.f);
		g_thePlayerCamera->SetPosition(m_position);
		g_thePlayerCamera->SetOrientation(m_orientation);
	}
	
}
void Player::Render()const {

}
Player::Player(Game* owner) :Entity(owner){
	m_position = Vec3(0.f, 0.f, 2.f);
}
Mat44 Player::GetTransform() {
	Mat44 modelToWOrld = Mat44::MakeTranslation3D(m_position);
	modelToWOrld.Append(m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
	return modelToWOrld;
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