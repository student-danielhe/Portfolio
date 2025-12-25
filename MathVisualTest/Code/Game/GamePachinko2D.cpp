#include "GamePachinko2D.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
void GamePachinko2D::StartUp() {
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	g_RNG = new RandomNumberGenerator();
	m_arrowHead = Vec2(10, 10);
	m_arrowTail = Vec2(0, 0);
	InitializeBumpers();
	m_physicsTimer = new Timer(m_fixedDeltaSeconds);
	m_physicsTimer->Start();
	m_textSize = 2.5f;
	m_dynamicDisc.reserve(sizeof(MobileDisc) * 1500);
	m_staticBox.reserve(sizeof(OBBBumper) * 30);
	m_staticCapsule.reserve(sizeof(CapsuleBumper) * 30);
	m_staticDisc.reserve(sizeof(DiscBumper) * 30);
}
void GamePachinko2D::Update(float deltaSecond) {
	if (m_usingPhysicsUpdate) {
		while (m_physicsTimer->DecrementPeriodIfElapsed()) {
			FixedUpdate(m_fixedDeltaSeconds);
		}
	}
	else {
		FixedUpdate(deltaSecond);
	}
	
	m_extraText = " B: Toggle Floor G/H: Change ball elasticity. \n [/]: Change Physics Update Time P: Toggle Physics Update  \n";
	m_extraText = m_extraText + "Elasticity: " + std::to_string(m_globalBounciness) + "Ball: "+std::to_string(m_dynamicDisc.size());
	HandleKeyPress(deltaSecond);
}
void GamePachinko2D::FixedUpdate(float deltaSeconds) {
	for (MobileDisc& disc : m_dynamicDisc) {
		disc.m_velocity.y -= 70.f * deltaSeconds;
		disc.m_center += deltaSeconds * disc.m_velocity;
	}
	for (MobileDisc& disc : m_dynamicDisc) {
		CollideWithMobileDisc(disc);
	}
	for (MobileDisc& disc : m_dynamicDisc) {
		CollideWithBox(disc);
		CollideWithDisc(disc);
		CollideWithCapsule(disc);
	}
	for (MobileDisc& disc : m_dynamicDisc) {
		CollideWithWorld(disc);
	}
}
void GamePachinko2D::Render() {
	//add verts for arrow
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	DebugDrawRing(m_arrowTail, 1.f, .1f, Rgba8(100, 100, 255));
	DebugDrawRing(m_arrowTail, 5.f, .1f, Rgba8(100, 100, 255));
	std::vector<Vertex_PCU> verts;
	AddVertsForArrow2D(verts, m_arrowTail, m_arrowHead, 5.f, Rgba8(0, 150, 0));
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(verts);
	RenderDiscBumpers();
	RenderCapsuleBumpers();
	RenderOBBBumpers();
	RenderMobileDisc();
	g_theRenderer->EndCamera(*g_ScreenCamera);


}
void GamePachinko2D::HandleKeyPress(float deltaSeconds) {
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
		Vec2 mouseUV = g_theWindow->GetNormalizedMouseUV();
		mouseUV.x = GetClamped(mouseUV.x, 0, 1);
		mouseUV.y = GetClamped(mouseUV.y, 0, 1);
		m_arrowTail = Vec2(mouseUV.x * WORLD_SIZE_X, mouseUV.y * WORLD_SIZE_Y);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE)) {
		Vec2 mouseUV = g_theWindow->GetNormalizedMouseUV();
		mouseUV.x = GetClamped(mouseUV.x, 0, 1);
		mouseUV.y = GetClamped(mouseUV.y, 0, 1);
		m_arrowHead = Vec2(mouseUV.x * WORLD_SIZE_X, mouseUV.y * WORLD_SIZE_Y);
	}
	//-------------------
	if (g_theInput->WasKeyJustPressed('T')) {
		m_physicsTimer->m_period = m_fixedDeltaSeconds *20.f;
	}
	else {
		m_physicsTimer->m_period = m_fixedDeltaSeconds;
	}
	if (g_theInput->WasKeyJustReleased('[')) {
		m_fixedDeltaSeconds *= 1.f / 1.1f;
		m_physicsTimer->m_period = m_fixedDeltaSeconds;
	}if (g_theInput->WasKeyJustReleased(']')) {
		m_fixedDeltaSeconds *= 1.1f;
		m_physicsTimer->m_period = m_fixedDeltaSeconds;
	}
	if (g_theInput->WasKeyJustReleased('P')) {
		m_usingPhysicsUpdate = !m_usingPhysicsUpdate;
	}
	if (g_theInput->WasKeyJustReleased('B')) {
		m_bottomWall = !m_bottomWall;
	}
	if (g_theInput->WasKeyJustReleased('G')) {
		m_globalBounciness -= .05f;
		if (m_globalBounciness < 0.f)
			m_globalBounciness = 0.f;
	}
	if (g_theInput->WasKeyJustReleased('H')) {
		m_globalBounciness += .05f;
		if (m_globalBounciness > 1.f)
			m_globalBounciness = 1.f;
	}
	if (g_theInput->WasKeyJustPressed('W')) {
		m_arrowTail.y += 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('S')) {
		m_arrowTail.y -= 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('D')) {
		m_arrowTail.x += 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('A')) {
		m_arrowTail.x -= 30 * deltaSeconds;
	}

	//------------
	if (g_theInput->WasKeyJustPressed('I')) {
		m_arrowHead.y += 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('K')) {
		m_arrowHead.y -= 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('J')) {
		m_arrowHead.x -= 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('L')) {
		m_arrowHead.x += 30 * deltaSeconds;
	}
	//-----------------
	if (g_theInput->WasKeyJustPressed(KEYCODE_UP)) {
		m_arrowHead.y += 30 * deltaSeconds;
		m_arrowTail.y += 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_DOWN)) {
		m_arrowHead.y -= 30 * deltaSeconds;
		m_arrowTail.y -= 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT)) {
		m_arrowHead.x -= 30 * deltaSeconds;
		m_arrowTail.x -= 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT)) {
		m_arrowHead.x += 30 * deltaSeconds;
		m_arrowTail.x += 30 * deltaSeconds;
	}
	if (g_theInput->WasKeyJustReleased(' ')|| g_theInput->WasKeyJustPressed('N')) {
		//spawn ball
		MobileDisc disc;
		disc.m_center = m_arrowTail;
		int bright = g_RNG->RollRandomIntInRange(0, 255);
		disc.m_color = Rgba8(bright, bright,255);
		disc.m_radius = g_RNG->RollRandomFloatInRange(1.f, 5.f);
		disc.m_velocity = 3.f * (m_arrowHead-m_arrowTail);
		m_dynamicDisc.push_back(disc);
	}
	//clamp to fit
	m_arrowHead.x = GetClamped(m_arrowHead.x, 0, WORLD_SIZE_X);
	m_arrowTail.x = GetClamped(m_arrowTail.x, 0, WORLD_SIZE_X);
	m_arrowHead.y = GetClamped(m_arrowHead.y, 0, WORLD_SIZE_Y);
	m_arrowTail.y = GetClamped(m_arrowTail.y, 0, WORLD_SIZE_Y);
}

void GamePachinko2D::InitializeBumpers() {
	for (int i = 0; i < 10; i++) {
		float discSize = g_RNG->RollRandomFloatInRange(10.f, 20.f);
		float capsuleSize = g_RNG->RollRandomFloatInRange(5.f, 10.f);
		float OBBLength = g_RNG->RollRandomFloatInRange(3.f, 15.f);
		float OBBWidth = g_RNG->RollRandomFloatInRange(3.f, 15.f);
		Vec2 discCenter(g_RNG->RollRandomFloatInRange(40.f,WORLD_SIZE_X-40.f), g_RNG->RollRandomFloatInRange(40.f,WORLD_SIZE_Y-40.f));
		Vec2 capsuleCenter1(g_RNG->RollRandomFloatInRange(40.f, WORLD_SIZE_X - 40.f), g_RNG->RollRandomFloatInRange(40.f, WORLD_SIZE_Y - 40.f));
		Vec2 capDir = Vec2(g_RNG->RollRandomFloatInRange(-1.f, 1.f), g_RNG->RollRandomFloatInRange(-1.f, 1.f));
		capDir.Normalize();
		Vec2 capsuleCenter2 = capsuleCenter1 + capDir * g_RNG->RollRandomFloatInRange(10.f, 20.f);
		Vec2 OBBCenter(g_RNG->RollRandomFloatInRange(40.f, WORLD_SIZE_X - 40.f), g_RNG->RollRandomFloatInRange(40.f, WORLD_SIZE_Y - 40.f));
		Vec2 OBBiBasis = Vec2(g_RNG->RollRandomFloatInRange(-1.f, 1.f), g_RNG->RollRandomFloatInRange(-1.f, 1.f));
		OBBiBasis.Normalize();

		//add to list
		OBBBumper newObb;
		newObb.m_box= OBB2(OBBCenter, Vec2(OBBLength, OBBWidth), OBBiBasis);
		newObb.m_bounciness = g_RNG->RollRandomFloatInRange(.01f, .99f);
		m_staticBox.push_back(newObb);
		CapsuleBumper newCap;
		newCap.m_head = capsuleCenter1; 
		newCap.m_tail = capsuleCenter2;
		newCap.m_radius = capsuleSize;
		newCap.m_bounciness = g_RNG->RollRandomFloatInRange(.01f, .99f);
		m_staticCapsule.push_back(newCap);
		DiscBumper newDisc;
		newDisc.m_center = discCenter;
		newDisc.m_radius = discSize;
		newDisc.m_bounciness = g_RNG->RollRandomFloatInRange(.01f, .99f);
		m_staticDisc.push_back(newDisc);
	}
}
void GamePachinko2D::RenderMobileDisc() {
	std::vector<Vertex_PCU> verts;
	for (MobileDisc disc : m_dynamicDisc) {
		AddVertsForDisc(verts, disc.m_center, disc.m_radius, disc.m_color, Vec2());
	}
	g_theRenderer->DrawVertexArray(verts);
}
void GamePachinko2D::RenderDiscBumpers() {
	std::vector<Vertex_PCU> verts;
	for (DiscBumper bumper : m_staticDisc) {
		Rgba8 color(RoundDownToInt(255 * (1-bumper.m_bounciness)), RoundDownToInt(255 * (bumper.m_bounciness)), 0);
		AddVertsForDisc(verts, bumper.m_center, bumper.m_radius, color,Vec2());
	}
	g_theRenderer->DrawVertexArray(verts);
}
void GamePachinko2D::RenderOBBBumpers() {
	std::vector<Vertex_PCU> verts;
	for (OBBBumper bumper: m_staticBox) {
		Rgba8 color(RoundDownToInt(255 * (1-bumper.m_bounciness)), RoundDownToInt(255 * (bumper.m_bounciness)), 0);
		Vec2 corners[4]; 
		bumper.m_box.GetCornerPoints(corners);
		AddVertsForRect(verts,corners[0],corners[1],corners[2],corners[3],color,Vec2());
	}
	g_theRenderer->DrawVertexArray(verts);
}
void GamePachinko2D::RenderCapsuleBumpers() {
	std::vector<Vertex_PCU> verts;
	for (CapsuleBumper bumper : m_staticCapsule) {
		Rgba8 color(RoundDownToInt(255 * (1-bumper.m_bounciness)), RoundDownToInt(255 * (bumper.m_bounciness)), 0);
		AddVertsForCapsule(verts, bumper.m_head,bumper.m_tail, bumper.m_radius, color, Vec2());
	}
	g_theRenderer->DrawVertexArray(verts);
}

void GamePachinko2D::CollideWithMobileDisc(MobileDisc& disc) {
	for (MobileDisc& otherDisc : m_dynamicDisc) {
		if (&otherDisc != &disc) {
			BounceDiscOffOther2D(disc.m_center, disc.m_radius, m_globalBounciness, disc.m_velocity,
				otherDisc.m_center, otherDisc.m_radius, m_globalBounciness, otherDisc.m_velocity);
		}
		
	}
}

void GamePachinko2D::CollideWithDisc(MobileDisc& disc) {
	for (DiscBumper otherDisc : m_staticDisc) {
		BounceDiscOffDisc2D(disc.m_center, disc.m_radius, m_globalBounciness, disc.m_velocity,
			otherDisc.m_center, otherDisc.m_radius, otherDisc.m_bounciness);
	}
}
void GamePachinko2D::CollideWithBox(MobileDisc& disc) {
	for (OBBBumper box : m_staticBox) {
		BounceDiscOffOBB2D(disc.m_center, disc.m_radius, m_globalBounciness, disc.m_velocity,
			box.m_box, box.m_bounciness);
	}
}
void GamePachinko2D::CollideWithCapsule(MobileDisc& disc) {
	for (CapsuleBumper capsule : m_staticCapsule) {
		BounceDiscOffCapsule2D(disc.m_center, disc.m_radius, m_globalBounciness, disc.m_velocity,
			capsule.m_head, capsule.m_tail, capsule.m_radius, capsule.m_bounciness);
	}
}
void GamePachinko2D::CollideWithWorld(MobileDisc& disc) {
	if (disc.m_center.x < disc.m_radius) {
		disc.m_center.x = disc.m_radius;
		Vec2 nearest(0.f,disc.m_center.y);
		BounceDiscOffPoint(disc.m_center, disc.m_radius, m_globalBounciness, disc.m_velocity, nearest);
	}
	if (disc.m_center.x > WORLD_SIZE_X - disc.m_radius) {
		disc.m_center.x = WORLD_SIZE_X - disc.m_radius;
		Vec2 nearest(WORLD_SIZE_X, disc.m_center.y);
		BounceDiscOffPoint(disc.m_center, disc.m_radius, m_globalBounciness, disc.m_velocity, nearest);
	}
	
	if (disc.m_center.y < disc.m_radius) {
		if (m_bottomWall) {
			disc.m_center.y = disc.m_radius;
			Vec2 nearest(disc.m_center.x, 0.f);
			BounceDiscOffPoint(disc.m_center, disc.m_radius, m_globalBounciness, disc.m_velocity, nearest);
		}
		else {
			disc.m_center.y = WORLD_SIZE_Y * 1.1f;
		}
	}

	
	/*if (disc.m_center.y > WORLD_SIZE_Y - disc.m_radius) {
		Vec2 nearest(disc.m_center.x, WORLD_SIZE_Y);
		BounceDiscOffPoint(disc.m_center, disc.m_radius, m_globalBounciness, disc.m_velocity, nearest);
	}*/
}