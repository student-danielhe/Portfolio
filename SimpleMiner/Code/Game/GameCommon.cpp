#include "GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
//#include "Engine/Math/RandomNumberGenerator.hpp"
void DebugDrawRing(Vec2 pos, float radius,float thickness, Rgba8 color) {
	constexpr int NUM_SIDES = 12;
	constexpr float DEG_PER_SIDE = 360.f / NUM_SIDES;
	Vec2 start(radius, 0);
	Vec2 end = start.GetRotatedDegrees(DEG_PER_SIDE);
	for (int i = 0; i < NUM_SIDES; i++) {
		DebugDrawLine(pos + start, pos + end, thickness, color);
		start.RotateDegrees(DEG_PER_SIDE);
		end.RotateDegrees(DEG_PER_SIDE);
	}

}
void DebugDrawLine(Vec2 start, Vec2 end, float thickness, Rgba8 color) {
	Vertex_PCU* arr = new Vertex_PCU[6];
	Vec2 uv (0, 0);
	Vec2 step = end - start;
	step.SetLength(thickness);
	Vec2 stepLeft = step.GetRotated90Degrees();
	Vec2 SL = start - step + stepLeft;
	Vec2 SR = start - step - stepLeft;
	Vec2 EL = end + step + stepLeft;
	Vec2 ER = end + step - stepLeft;
	arr[0].m_position = Vec3(SL.x, SL.y, 0);
	arr[1].m_position = Vec3(SR.x, SR.y, 0);
	arr[2].m_position = Vec3(ER.x, ER.y, 0);
	arr[3].m_position = Vec3(EL.x, EL.y, 0);
	arr[4].m_position = Vec3(ER.x, ER.y, 0);
	arr[5].m_position = Vec3(SL.x, SL.y, 0);
	for (int i = 0; i < 6; i++) {
		arr[i].m_color = color;
		arr[i].m_uvTexCoords = uv;
	}
	g_theRenderer->DrawVertexArray(6, arr);
}
int  GetConnectedControllerID() {
	for (int i = 0; i < 4; i++) {
		if (g_theInput->GetController(i).IsConnected()) {
			return i;
		}
	}
	return 0;
}

void AddVertsForWorldBasis(std::vector<Vertex_PCU>& verts, float size)
{
    AddVertsForArrow3D(verts, Vec3(), Vec3(size, 0.f, 0.f), size/10.f, Rgba8::RED, 4);

	AddVertsForArrow3D(verts, Vec3(), Vec3(0.f, size, 0.f), size/10.f, Rgba8::GREEN, 4);

	AddVertsForArrow3D(verts, Vec3(), Vec3(0.f, 0.f, size), size/10.f, Rgba8::BLUE, 4);
}
