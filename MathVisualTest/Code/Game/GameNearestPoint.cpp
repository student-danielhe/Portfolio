#include"GameNearestPoint.hpp"
#include "Engine/core/Vertex_PCU.hpp"
#include <vector>
void GameNearestPoint::Render() {
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	//RenderDisc(Vec2(WORLD_SIZE_X / 4, WORLD_SIZE_Y / 4), 10);
	//RenderLineInfinite(Vec2(0,WORLD_SIZE_Y/2),Vec2(1,WORLD_SIZE_Y/2));
	//RenderRect(Vec2(WORLD_SIZE_X*3/4,WORLD_SIZE_Y/4),Vec2(WORLD_SIZE_X*3/4,WORLD_SIZE_Y*3/4),);
	//RenderCapsule(Vec2(WORLD_SIZE_X, WORLD_SIZE_Y / 4), Vec2(WORLD_SIZE_X * 3 / 4, WORLD_SIZE_Y * 3 / 4), 10);
	Rgba8 blue = Rgba8(0, 0, 255);
	Rgba8 lightBlue = Rgba8(150, 150, 255);
	Rgba8 gold = Rgba8(255, 200, 100);
	Rgba8 white = Rgba8(255, 255, 255);
	std::vector<Vertex_PCU> arr;
	Vec2 UV = Vec2(0, 0);
	//Render Shapes
	if (IsPointInsideDisc2D(m_player, m_discCenter, m_discRadius)) {
		AddVertsForDisc(arr,m_discCenter, m_discRadius,lightBlue,UV);
	}
	else {
		AddVertsForDisc(arr,m_discCenter, m_discRadius, blue,UV);
	}
	Vec2 BR = Vec2(m_AABB2->m_maxs.x, m_AABB2->m_mins.y);
	Vec2 TL = Vec2(m_AABB2->m_mins.x, m_AABB2->m_maxs.y);
	if (m_AABB2->IsPointInside(m_player)) {
		AddVertsForRect(arr,BR, m_AABB2->m_maxs, TL, m_AABB2->m_mins, lightBlue,UV);
	}
	else {
		AddVertsForRect(arr,BR, m_AABB2->m_maxs, TL, m_AABB2->m_mins, blue,UV);
	}
	AddVertsForLine(arr, m_segStart, m_segEnd, blue, UV);

	Vec2 normal = (m_infEnd - m_infStart).GetNormalized();
	float length = WORLD_SIZE_X + WORLD_SIZE_Y;
	AddVertsForLine(arr,m_infStart - (normal * length), m_segEnd +(normal * length),  blue,UV);

	if (IsPointInsideCapsule2D(m_player, m_boneStart, m_boneEnd, m_capsuleRad)) {
		AddVertsForCapsule(arr,m_boneStart, m_boneEnd, m_capsuleRad, lightBlue,UV);
	}
	else {
		AddVertsForCapsule(arr,m_boneStart, m_boneEnd, m_capsuleRad, blue,UV);
	}
	if (IsPointInsideTriangle2D(m_player, m_triA, m_triB, m_triC)) {
		AddVertsForTriangle(arr,m_triA, m_triB, m_triC, lightBlue,UV);
	}
	else {
		AddVertsForTriangle(arr,m_triA, m_triB, m_triC, blue,UV);
	}
	
	Vec2* corner=new Vec2[4];
	m_OBB2->GetCornerPoints(corner);
	if (IsPointInsideOBB2D(m_player, *m_OBB2)) {
		AddVertsForRect(arr,corner[0], corner[1], corner[2],corner[3], lightBlue,UV);
	}
	else {
		AddVertsForRect(arr,corner[0], corner[1], corner[2], corner[3], blue,UV);
	}
	
	

	//Render Nearest Points
	Vec2 point = GetNearestPointOnDisc2D(m_player, m_discCenter, m_discRadius);
	AddVertsForDisc(arr,point, 3, gold,UV);
	DebugDrawLine(m_player, point, 0.5, white);
	point = m_AABB2->GetNearestPoint(m_player);
	AddVertsForDisc(arr, point, 3, gold, UV);
	DebugDrawLine(m_player, point, 0.5, white);
	point = GetNearestPointOnLineSegment2D(m_player, m_segStart, m_segEnd);
	AddVertsForDisc(arr, point, 3, gold, UV);
	DebugDrawLine(m_player, point, 0.5, white);
	point = GetNearestPointOnInfiniteLine2D(m_player, m_infStart, m_infEnd);
	AddVertsForDisc(arr, point, 3, gold, UV);
	DebugDrawLine(m_player, point, 0.5, white);
	point = GetNearestPointOnCapsule2D(m_player, m_boneStart, m_boneEnd,m_capsuleRad);
	AddVertsForDisc(arr, point, 3, gold, UV);
	DebugDrawLine(m_player, point, 0.5, white);
	point = GetNearestPointOnTriangle2D(m_player, m_triA, m_triB, m_triC);
	AddVertsForDisc(arr, point, 3, gold, UV);
	DebugDrawLine(m_player, point, 0.5, white);
	point = GetNearestPointOnOBB2D(m_player, *m_OBB2);
	AddVertsForDisc(arr, point, 3, gold, UV);
	DebugDrawLine(m_player, point, 0.5, white);

	
	
	//Render Player
	AddVertsForDisc(arr,m_player, 2,Rgba8(255,255,255),UV);
	g_theRenderer->DrawVertexArray(arr);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void GameNearestPoint::StartUp() {
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	g_RNG = new RandomNumberGenerator();
	//"Player"
	m_player = Vec2(WORLD_SIZE_X/2,WORLD_SIZE_Y/2);
	//Disc initialize
	m_discCenter =Vec2(g_RNG->RollRandomFloatInRange(20,380),g_RNG->RollRandomFloatInRange(20,180));
	m_discRadius = g_RNG->RollRandomFloatInRange(3,20);
	//AABB2
	m_AABB2 = new AABB2(Vec2(g_RNG->RollRandomFloatInRange(20, 180), g_RNG->RollRandomFloatInRange(20, 80)), Vec2(g_RNG->RollRandomFloatInRange(200, 380), g_RNG->RollRandomFloatInRange(100, 180)));
	//Line
	m_infStart = Vec2(g_RNG->RollRandomFloatInRange(20, 380), g_RNG->RollRandomFloatInRange(20, 180));
	m_infEnd = Vec2(g_RNG->RollRandomFloatInRange(20, 380), g_RNG->RollRandomFloatInRange(20, 180));
	m_segStart = Vec2(g_RNG->RollRandomFloatInRange(20, 380), g_RNG->RollRandomFloatInRange(20, 180));
	m_segEnd = Vec2(g_RNG->RollRandomFloatInRange(20, 380), g_RNG->RollRandomFloatInRange(20, 180));
	//capsule
	m_boneStart = Vec2(g_RNG->RollRandomFloatInRange(20, 380), g_RNG->RollRandomFloatInRange(20, 180));
	m_boneEnd = Vec2(g_RNG->RollRandomFloatInRange(20, 380), g_RNG->RollRandomFloatInRange(20, 180));
	m_capsuleRad = g_RNG->RollRandomFloatInRange(3,20);
	//triangle
	m_triA = Vec2(g_RNG->RollRandomFloatInRange(20, 30), g_RNG->RollRandomFloatInRange(20, 30));
	m_triB = Vec2(g_RNG->RollRandomFloatInRange(50, 80), g_RNG->RollRandomFloatInRange(40, 70));
	m_triC = Vec2(g_RNG->RollRandomFloatInRange(20, 30), g_RNG->RollRandomFloatInRange(80, 100));
	//OBB2
	m_OBB2 = new OBB2(Vec2(g_RNG->RollRandomFloatInRange(120, 280), g_RNG->RollRandomFloatInRange(60, 120)),Vec2(g_RNG->RollRandomFloatInRange(3, 20), g_RNG->RollRandomFloatInRange(3, 20)),Vec2(1,0).GetRotatedDegrees(g_RNG->RollRandomFloatInRange(0,360)));
}
void GameNearestPoint::Update(float DeltaSeconds) {
	if (g_theInput->WasKeyJustPressed('W')) {
		m_player.y += 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('S')) {
		m_player.y -= 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('D')) {
		m_player.x += 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed('A')) {
		m_player.x -= 30 * DeltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
		Vec2 mouseUV = g_theWindow->GetNormalizedMouseUV();
		mouseUV.x = GetClamped(mouseUV.x, 0, 1);
		mouseUV.y = GetClamped(mouseUV.y, 0, 1);
		m_player = Vec2(mouseUV.x * WORLD_SIZE_X, mouseUV.y * WORLD_SIZE_Y);
	}
}
