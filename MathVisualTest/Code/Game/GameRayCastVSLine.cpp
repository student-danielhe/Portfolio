#include "GameRayCastVSLine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
void GameRayCastVSLine::Render() {
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	std::vector<Vertex_PCU> verts;
	RaycastResult2D firstLine;
	firstLine.m_didImpact = false;
	firstLine.m_impactDist = INFINITY;
	firstLine.m_impactNormal = Vec2(0, 0);
	firstLine.m_impactPos = Vec2(0, 0);
	int firstLineIndex = -1;

	for (int i = 0; i < m_LineStarts.size(); i++) {
		
		RaycastResult2D result = RaycastVsLineSegment2D(m_arrowTail, (m_arrowHead - m_arrowTail).GetNormalized(), (m_arrowTail - m_arrowHead).GetLength(), m_LineStarts[i], m_LineEnds[i]);
		if (result.m_didImpact) {
			//find first disc hit
			if (result.m_impactDist < firstLine.m_impactDist) {
				firstLine = result;
				firstLineIndex = i;
			}
		}
		AddVertsForLine(verts, m_LineStarts[i], m_LineEnds[i], Rgba8(0, 0, 255), Vec2(0, 0));
	}
	//light up first disc
	if (firstLine.m_didImpact) {
		AddVertsForLine(verts, m_LineStarts[firstLineIndex], m_LineEnds[firstLineIndex], Rgba8(200, 200, 255), Vec2(0, 0));
	}

	//add verts for arrow
	AddVertsForArrow2D(verts, m_arrowTail, m_arrowHead, 10, Rgba8(0, 150, 0));

	//add stuff if there is a first disc
	if (firstLine.m_didImpact) {
		AddVertsForArrow2D(verts, m_arrowTail, firstLine.m_impactPos, 10, Rgba8(255, 255, 255));
		AddVertsForArrow2D(verts, firstLine.m_impactPos, firstLine.m_impactPos + firstLine.m_impactNormal * 50, 10, Rgba8(0, 255, 0));
	}

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(verts);
	//delete(verts);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void GameRayCastVSLine::StartUp() {
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	g_RNG = new RandomNumberGenerator();
	m_arrowHead = Vec2(10, 10);
	m_arrowTail = Vec2(0, 0);
	//initialize discs

	for (int i = 0; i < 12; i++) {
		m_LineStarts.push_back(Vec2(g_RNG->RollRandomFloatInRange(50.f, WORLD_SIZE_X - 50.f), g_RNG->RollRandomFloatInRange(50.f, WORLD_SIZE_Y - 50.f)));
		m_LineEnds.push_back(Vec2(g_RNG->RollRandomFloatInRange(m_LineStarts[i].x - 50.f, m_LineStarts[i].x + 50.f), g_RNG->RollRandomFloatInRange(m_LineStarts[i].y - 50.f, m_LineStarts[i].y + 50.f)));
	}


}
void GameRayCastVSLine::Update(float deltaSecond) {
	HandleKeyPress(deltaSecond);
}
void GameRayCastVSLine::HandleKeyPress(float DeltaSeconds) {
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
	m_arrowHead.x = GetClamped(m_arrowHead.x, 0, WORLD_SIZE_X);
	m_arrowTail.x = GetClamped(m_arrowTail.x, 0, WORLD_SIZE_X);
	m_arrowHead.y = GetClamped(m_arrowHead.y, 0, WORLD_SIZE_Y);
	m_arrowTail.y = GetClamped(m_arrowTail.y, 0, WORLD_SIZE_Y);

}