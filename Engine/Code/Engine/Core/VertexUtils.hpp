#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine//Math/AABB2.hpp"
#include "Engine//Math/AABB3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/CubicBeizerCurve2D.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Plane.hpp"
#include <vector>
//untextured
void AddVertsForTriangle(std::vector<Vertex_PCU> &arr, Vec2 const& A, Vec2 const& B, Vec2 const& C, Rgba8 const& color, Vec2 const& UV);
void AddVertsForDisc(std::vector<Vertex_PCU> &arr, Vec2 const& center, float radius, Rgba8 const& color, Vec2 const& UV);
void AddVertsForRect(std::vector<Vertex_PCU> &arr, Vec2 const& BR, Vec2 const& TR, Vec2 const& TL, Vec2 const& BL, Rgba8 const& color, Vec2 const& UV);
void AddVertsForPizza(std::vector<Vertex_PCU> &arr, Vec2 const& center, float radius, float startDeg, float endDeg, Rgba8 const& color, Vec2 const& UV);
void AddVertsForCapsule(std::vector<Vertex_PCU> &arr, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color, Vec2 const& UV);
void AddVertsForLine(std::vector<Vertex_PCU> &arr, Vec2 const& start, Vec2 const& end, Rgba8 const& color, Vec2 const& UV, float thickness=1.f);
//textured
void AddVertsForTriangle(std::vector<Vertex_PCU>& arr, Vertex_PCU const& A, Vertex_PCU const& B, Vertex_PCU const& C);
void AddVertsForRect(std::vector<Vertex_PCU>& arr, Vertex_PCU const& BR, Vertex_PCU const& TR, Vertex_PCU const& TL, Vertex_PCU const& BL);

void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float arrowSize, Rgba8 const& color);
void AddVertsForRect(std::vector<Vertex_PCU>& arr, Vec2 const& BR, Vec2 const& TR, Vec2 const& TL, Vec2 const& BL, Rgba8 const& color, AABB2 const& UV);
void AddVertsForRect(std::vector<Vertex_PCU>& arr, AABB2 box, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UV= AABB2::DEFAULT);
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color = Rgba8(255, 255, 255), const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)));

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color = Rgba8(255, 255, 255), const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)));

void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color = Rgba8(255, 255, 255), const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)));


void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, const Mat44& transform);
void TransformVertexArray3D(std::vector<Vertex_PCUTBN>& verts, const Mat44& transform);

AABB2 GetVertexBounds2D(const std::vector<Vertex_PCU>& verts);
void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts,
	const Vec3& start, const Vec3& end, float radius,
	const Rgba8& color = Rgba8(255, 255, 255),
	const AABB2& UV = AABB2(Vec2(0.f,0.f),Vec2(1.f,1.f)), int numSlice = 32);

void AddVertsForZCylinder3D(std::vector<Vertex_PCUTBN>& verts,
	std::vector<unsigned int>& indexes,
	const Vec3& start, float height, float radius,
	const Rgba8& color = Rgba8(255, 255, 255),
	const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)), int numSlice = 32);

void AddVertsForCone3D(std::vector<Vertex_PCU>& verts,
	const Vec3& start, const Vec3& end, float radius,
	const Rgba8& color = Rgba8(255, 255, 255),
	const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)), int numSlice = 32);
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts,
	const Vec3& start, const Vec3& end, float radius,
	const Rgba8& color = Rgba8(255, 255, 255), int numSlice = 32);

void AddVertsForSphere3D(std::vector<Vertex_PCU>&verts, 
	float radius, 
	const Rgba8& color = Rgba8(255, 255, 255), 
	const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)), 
	int numSlice = 32, int numStack = 16);

void AddVertsForSphere3D(std::vector<Vertex_PCUTBN>& verts,
	std::vector<unsigned int>& indexes,
	Vec3 center, float radius,
	const Rgba8& color = Rgba8(255, 255, 255),
	const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)),
	int numSlice = 32, int numStack = 16);

void AddVertsForAABB3D(std::vector<Vertex_PCU>&verts, 
	const AABB3& cube, 
	const Rgba8& color = Rgba8(255, 255, 255), 
	const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)) );

void AddVertsForAABB3D(std::vector<Vertex_PCUTBN>& verts,
	std::vector<unsigned int>& indexes,
	const AABB3& cube,
	const Rgba8& color = Rgba8(255, 255, 255),
	const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)));

void AddVertsForZCylinder3D(std::vector<Vertex_PCU>& verts,
	const Vec3& bottom, float height, float radius,
	const Rgba8& color = Rgba8(255, 255, 255),
	const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)), int numSlice = 32);

void AddVertsForCubicBeizer(std::vector<Vertex_PCU>& verts, CubicBeizerCurve2D curve, Rgba8 color, int numSlice = 64);

void AddVertsForOBB3D(std::vector<Vertex_PCU>& verts,
	const OBB3& cube,
	const Rgba8& color = Rgba8(255, 255, 255),
	const AABB2& UV = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)));
void AddVertsForPlane3D(std::vector<Vertex_PCU>& verts,
	const Plane3& plane);