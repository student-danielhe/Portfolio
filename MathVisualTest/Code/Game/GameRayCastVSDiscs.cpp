#include "GameRayCastVSDiscs.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
void GameRayCastVSDiscs::Render() {
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	std::vector<Vertex_PCU> verts;
	RaycastResult2D firstDisc;
	firstDisc.m_didImpact = false;
	firstDisc.m_impactDist = INFINITY;
	firstDisc.m_impactNormal = Vec2(0, 0);
	firstDisc.m_impactPos = Vec2(0, 0);
	int firstDiscIndex = -1;
	
	for (int i = 0; i < m_discCenters.size(); i++) {
		//RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius);
		RaycastResult2D result = RaycastVsDisc2D(m_arrowTail, (m_arrowHead - m_arrowTail).GetNormalized(), (m_arrowTail - m_arrowHead).GetLength(), m_discCenters[i], m_discRadius[i]);
		if (result.m_didImpact) {
			//find first disc hit
			if (result.m_impactDist < firstDisc.m_impactDist) {
				firstDisc = result;
				firstDiscIndex = i;
			}
		}
		AddVertsForDisc(verts, m_discCenters[i], m_discRadius[i], Rgba8(0, 0, 255), Vec2(0, 0));
	}
	//light up first disc
	if (firstDisc.m_didImpact) {
		AddVertsForDisc(verts, m_discCenters[firstDiscIndex], m_discRadius[firstDiscIndex], Rgba8(200, 200, 255), Vec2(0, 0));
	}

	//add verts for arrow
	AddVertsForArrow2D(verts, m_arrowTail, m_arrowHead, 3.f, Rgba8(0, 150, 0));

	//add stuff if there is a first disc
	if (firstDisc.m_didImpact) {
		AddVertsForArrow2D(verts, m_arrowTail, firstDisc.m_impactPos, 10, Rgba8(255, 255, 255));
		AddVertsForArrow2D(verts, firstDisc.m_impactPos, firstDisc.m_impactPos + firstDisc.m_impactNormal*50, 10, Rgba8(0, 255, 0));
	}
	
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(verts);
	//delete(verts);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void GameRayCastVSDiscs::StartUp() {
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	g_RNG = new RandomNumberGenerator();
	m_arrowHead = Vec2(10, 10);
	m_arrowTail = Vec2(0, 0);
	//initialize discs

	for (int i = 0; i < 12; i++) {
		m_discCenters.push_back(Vec2(g_RNG->RollRandomFloatInRange(50,WORLD_SIZE_X-50), g_RNG->RollRandomFloatInRange(50,WORLD_SIZE_Y-50)));
		m_discRadius.push_back(g_RNG->RollRandomFloatInRange(10,50));
	}
	
	
}
void GameRayCastVSDiscs::Update(float deltaSecond) {
	HandleKeyPress(deltaSecond);
}
void GameRayCastVSDiscs::HandleKeyPress(float DeltaSeconds) {
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
	if (g_theInput->WasKeyJustPressed('W')) {
		m_arrowTail.y += 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('S')) {
		m_arrowTail.y -= 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('D')) {
		m_arrowTail.x += 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('A')) {
		m_arrowTail.x -= 30 * DeltaSeconds;
	}

	//------------
	if (g_theInput->WasKeyJustPressed('I')) {
		m_arrowHead.y += 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('K')) {
		m_arrowHead.y -= 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('J')) {
		m_arrowHead.x -= 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('L')) {
		m_arrowHead.x += 30 * DeltaSeconds;
	}
	//-----------------
	if (g_theInput->WasKeyJustPressed(KEYCODE_UP)) {
		m_arrowHead.y += 30 * DeltaSeconds;
		m_arrowTail.y += 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_DOWN)) {
		m_arrowHead.y -= 30 * DeltaSeconds;
		m_arrowTail.y -= 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT)) {
		m_arrowHead.x -= 30 * DeltaSeconds;
		m_arrowTail.x -= 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT)) {
		m_arrowHead.x += 30 * DeltaSeconds;
		m_arrowTail.x += 30 * DeltaSeconds;
	}
	//clamp to fit
	m_arrowHead.x = GetClamped(m_arrowHead.x,0, WORLD_SIZE_X);
	m_arrowTail.x = GetClamped(m_arrowTail.x, 0, WORLD_SIZE_X);
	m_arrowHead.y = GetClamped(m_arrowHead.y, 0, WORLD_SIZE_Y);
	m_arrowTail.y = GetClamped(m_arrowTail.y, 0, WORLD_SIZE_Y);

}