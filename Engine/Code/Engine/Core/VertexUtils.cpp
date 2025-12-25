#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>
void AddVertsForTriangle(std::vector<Vertex_PCU> &arr, Vec2 const& A, Vec2 const& B, Vec2 const& C, Rgba8 const& color, Vec2 const& UV) {
	std::vector<Vertex_PCU> triangle;
	for (int i = 0; i < 3; i++) {
		triangle.push_back(Vertex_PCU(Vec3(),color,UV));
	}
	triangle[0].m_position = Vec3(A.x, A.y, 0);
	triangle[1].m_position = Vec3(B.x, B.y, 0);
	triangle[2].m_position = Vec3(C.x, C.y, 0);
	arr.push_back(triangle[0]);
	arr.push_back(triangle[1]);
	arr.push_back(triangle[2]);
	triangle.clear();
	
}
void AddVertsForDisc(std::vector<Vertex_PCU> &arr, Vec2 const& center, float radius, Rgba8 const& color, Vec2 const& UV) {
	constexpr int NUM_SIDES = 64;
	constexpr float DEG_PER_SIDE = 360.f / NUM_SIDES;
	Vec2 start(radius, 0);
	Vec2 end = start.GetRotatedDegrees(DEG_PER_SIDE);
	for (int i = 0; i < NUM_SIDES; i++) {
		AddVertsForTriangle(arr,center, center + start, center + end, color, UV);
		start = end;
		end.RotateDegrees(DEG_PER_SIDE);
	}
}

void AddVertsForRect(std::vector<Vertex_PCU> &arr, Vec2 const& BR, Vec2 const& TR, Vec2 const& TL, Vec2 const& BL, Rgba8 const& color, Vec2 const& UV) {
	AddVertsForTriangle(arr,BR, TR, TL, color, UV);
	AddVertsForTriangle(arr,BR, TL, BL, color, UV);
}

void AddVertsForRect(std::vector<Vertex_PCU>& arr, AABB2 box, Rgba8 const& color, AABB2 const& UV)
{
    AddVertsForRect(arr, Vec2(box.m_maxs.x,box.m_mins.y), box.m_maxs, Vec2(box.m_mins.x,box.m_maxs.y),box.m_mins, color, UV);
}

void AddVertsForPizza(std::vector<Vertex_PCU> &arr,Vec2 const& center, float radius, float startDeg, float endDeg, Rgba8 const& color, Vec2 const& UV) {
	constexpr int NUM_SIDES = 64;
	if (endDeg <= startDeg) {
		endDeg += 360;
	}
	float DEG_PER_SIDE = (endDeg - startDeg) / NUM_SIDES;
	Vec2 start(radius, 0);
	start.SetOrientationDegrees(startDeg);
	Vec2 end = start.GetRotatedDegrees(DEG_PER_SIDE);
	for (int i = 0; i < NUM_SIDES; i++) {
		AddVertsForTriangle(arr, center, center + start, center + end, color, UV);
		start = end;
		end.RotateDegrees(DEG_PER_SIDE);
	}
}
void AddVertsForCapsule(std::vector<Vertex_PCU> &arr, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color, Vec2 const& UV) {
	Vec2 normal = (boneEnd - boneStart).GetNormalized();
	float degStart = normal.GetOrientationDegrees() - 90;
	float degEnd = normal.GetOrientationDegrees() + 90;
	Vec2 BR = boneStart + normal.GetRotatedMinus90Degrees() * radius;
	Vec2 TR = boneEnd + normal.GetRotatedMinus90Degrees() * radius;
	Vec2 TL = boneEnd + normal.GetRotated90Degrees() * radius;
	Vec2 BL = boneStart + normal.GetRotated90Degrees() * radius;
	AddVertsForRect(arr,BR, TR, TL, BL, color, UV);
	AddVertsForPizza(arr, boneStart, radius, degEnd, degStart, color, UV);
	AddVertsForPizza(arr, boneEnd, radius, degStart, degEnd, color, UV);
}

void AddVertsForLine(std::vector<Vertex_PCU> &arr, Vec2 const& start, Vec2 const& end, Rgba8 const& color, Vec2 const& UV, float thickness) {
	Vec2 localLeft = ((end - start).GetNormalized()*thickness).GetRotated90Degrees();
	AddVertsForRect(arr,start - localLeft,end-localLeft,end+localLeft,start+localLeft,color,UV );
}

//---------------textured-------------------------
void AddVertsForTriangle(std::vector<Vertex_PCU>& arr, Vertex_PCU const& A, Vertex_PCU const& B, Vertex_PCU const& C) {
	arr.push_back(A);
	arr.push_back(B);
	arr.push_back(C);
}
void AddVertsForRect(std::vector<Vertex_PCU>& arr, Vertex_PCU const& BR, Vertex_PCU const& TR, Vertex_PCU const& TL, Vertex_PCU const& BL) {
	AddVertsForTriangle(arr, BR, TR, TL);
	AddVertsForTriangle(arr, BR, TL, BL);
}

void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float arrowSize, Rgba8 const& color) {
	AddVertsForLine(verts, start, end, color,Vec2(0,0), arrowSize / 10.f);

	Vec2 ArrowLeft = end+(end - start).GetNormalized().GetRotatedDegrees(135) * arrowSize;
	Vec2 ArrowRight = end+(end - start).GetNormalized().GetRotatedDegrees(225) * arrowSize;
	AddVertsForLine(verts, ArrowLeft, end, color, Vec2(0, 0), arrowSize/10.f);
	AddVertsForLine(verts, ArrowRight, end, color, Vec2(0, 0), arrowSize / 10.f);

}

void AddVertsForRect(std::vector<Vertex_PCU>& arr, Vec2 const& BR, Vec2 const& TR, Vec2 const& TL, Vec2 const& BL, Rgba8 const& color, AABB2 const& UV) {
	Vertex_PCU const& BRV =Vertex_PCU(Vec3(BR.x,BR.y,0), color, Vec2(UV.m_maxs.x,UV.m_mins.y));
	Vertex_PCU const& TRV= Vertex_PCU(Vec3(TR.x, TR.y, 0), color, UV.m_maxs);
	Vertex_PCU const& TLV= Vertex_PCU(Vec3(TL.x, TL.y, 0), color, Vec2(UV.m_mins.x, UV.m_maxs.y));
	Vertex_PCU const& BLV= Vertex_PCU(Vec3(BL.x, BL.y, 0), color, UV.m_mins);
	AddVertsForTriangle(arr, BRV, TRV, TLV);
	AddVertsForTriangle(arr, BRV, TLV, BLV);
}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts,const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color, const AABB2& UV) {
	Vertex_PCU const& BLV = Vertex_PCU(Vec3(bottomLeft.x, bottomLeft.y, bottomLeft.z), color, Vec2(UV.m_maxs.x, UV.m_mins.y));
	Vertex_PCU const& BRV = Vertex_PCU(Vec3(bottomRight.x, bottomRight.y, bottomRight.z), color, Vec2(UV.m_mins.x, UV.m_mins.y));
	Vertex_PCU const& TRV = Vertex_PCU(Vec3(topRight.x, topRight.y, topRight.z), color, Vec2(UV.m_mins.x, UV.m_maxs.y));
	Vertex_PCU const& TLV = Vertex_PCU(Vec3(topLeft.x, topLeft.y, topLeft.z), color, Vec2(UV.m_maxs.x,UV.m_maxs.y));
	AddVertsForTriangle(verts, BLV, BRV, TRV);
	AddVertsForTriangle(verts, TRV, TLV, BLV);
}

void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, const Mat44& transform) {
	for (Vertex_PCU& vertex : verts) {
		vertex.m_position = transform.TransformPosition3D(vertex.m_position);
	}
}

void TransformVertexArray3D(std::vector<Vertex_PCUTBN>& verts, const Mat44& transform) {
	for (Vertex_PCUTBN& vertex : verts) {
		vertex.m_position = transform.TransformPosition3D(vertex.m_position);
	}
}
AABB2 GetVertexBounds2D(const std::vector<Vertex_PCU>& verts) {
	AABB2 result;
	bool initialized = false;
	for (Vertex_PCU vertex : verts) {
		Vec2 pos(vertex.m_position.x, vertex.m_position.y);
		if (!initialized) {
			initialized = true;
			result.m_mins = pos;
			result.m_maxs = pos;
		}
		else {
			result.StretchToIncludePoint(pos);
		}

	}
	return result;
}
void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, const AABB2& UV, int numSlice) {
	float UVX = UV.m_maxs.x - UV.m_mins.x;
	float UVY = UV.m_maxs.y - UV.m_mins.y;
	float degPerSlice = 360.f / numSlice;
	std::vector<Vertex_PCU>localVerts;
	float len = (end - start).GetLength();
	for (int i = 0; i < numSlice; i++) {
		
		Vec3 BR = Vec3(0.f, CosDegrees(degPerSlice * i)*radius, SinDegrees(degPerSlice * i)*radius);
		Vec3 BL = Vec3(0.f, CosDegrees(degPerSlice * (i+1)) * radius, SinDegrees(degPerSlice * (i+1))*radius);
		Vec3 TR = Vec3(len,0.f,0.f)+ Vec3(0.f, CosDegrees(degPerSlice * i) * radius, SinDegrees(degPerSlice * i)*radius);
		Vec3 TL = Vec3(len,0.f,0.f)+ Vec3(0.f, CosDegrees(degPerSlice * (i+1.f)) * radius, SinDegrees(degPerSlice * (i+1.f))*radius);
		//add bottom triangle
		localVerts.push_back(Vertex_PCU(Vec3(0.f,0.f,0.f), color, Vec2(UVX / 2.f + UV.m_mins.x, UVY / 2.f + UV.m_mins.y)));
		localVerts.push_back(Vertex_PCU(BR, color, Vec2(CosDegrees(degPerSlice * (float)i)*UVX/2+UVX/2, SinDegrees(degPerSlice * (float)i) * UVY / 2+UVY/2 )));
		localVerts.push_back(Vertex_PCU(BL,color,Vec2(CosDegrees(degPerSlice * ((float)i+1.f)) * UVX/2+UVX/2, SinDegrees(degPerSlice * (float)(i+1.f)) * UVY / 2+UVY/2 )));

		//add top triangle
		localVerts.push_back(Vertex_PCU(Vec3(len,0.f,0.f), color, Vec2(UVX / 2.f + UV.m_mins.x, UVY / 2.f)));
		localVerts.push_back(Vertex_PCU(TL, color, Vec2(CosDegrees(degPerSlice * ((float)i+1.f)) * UVX/2+UVX/2, SinDegrees(degPerSlice * (float)(i + 1.f)) * UVY / 2+UVY/2)));
		localVerts.push_back(Vertex_PCU(TR, color, Vec2(CosDegrees(degPerSlice * (float)i) * UVX/2+UVX/2, SinDegrees(degPerSlice * (float)i) * UVY / 2 +UVY/2)));
		//add side
		AddVertsForQuad3D(localVerts, BL, BR, TR, TL, color, AABB2(Vec2(((float)i/(float)numSlice)*UVX+UV.m_mins.x, UV.m_mins.y), Vec2((((float)i+1.f) / (float)numSlice) * UVX + UV.m_mins.x, UV.m_maxs.y)));
	}

	Mat44 transform = Mat44::MakeTranslation3D(start);
	if (start.x == end.x && start.y == end.y) {
		Vec3 newEnd = end;
		newEnd += Vec3(.0001f, .0001f, 0.f);
		transform.Append(Mat44::MakeLookAtTransform(start, newEnd));
	}
	else {
		transform.Append(Mat44::MakeLookAtTransform(start, end));
	}
	
	
	TransformVertexArray3D(localVerts, transform);
	for (Vertex_PCU vertex : localVerts) {
		verts.push_back(vertex);
	}
}
void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, const AABB2& UV, int numSlice) {
	float degPerSlice = 360.f / numSlice;
	std::vector<Vertex_PCU>localVerts;
	float len = (end - start).GetLength();
	for (int i = 0; i < numSlice; i++) {

		Vec3 BR = Vec3(0.f, CosDegrees(degPerSlice * i) * radius, SinDegrees(degPerSlice * i) * radius);
		Vec3 BL = Vec3(0.f, CosDegrees(degPerSlice * (i + 1)) * radius, SinDegrees(degPerSlice * (i + 1)) * radius);
		//add bottom triangle
		localVerts.push_back(Vertex_PCU(Vec3(0.f, 0.f, 0.f), color, Vec2((UV.m_mins.x + UV.m_maxs.x) / 2, UV.m_maxs.y)));
		localVerts.push_back(Vertex_PCU(BR, color, Vec2(UV.m_maxs.x, UV.m_mins.y)));
		localVerts.push_back(Vertex_PCU(BL, color, UV.m_mins));

		//add top triangle
		localVerts.push_back(Vertex_PCU(Vec3(len, 0.f, 0.f), color, Vec2((UV.m_mins.x + UV.m_maxs.x) / 2, UV.m_maxs.y)));
		localVerts.push_back(Vertex_PCU(BL, color, UV.m_mins));
		localVerts.push_back(Vertex_PCU(BR, color, Vec2(UV.m_maxs.x, UV.m_mins.y)));
	}

	Mat44 transform = Mat44::MakeTranslation3D(start);
	if (start.x == end.x && start.y == end.y) {
		Vec3 newEnd = end;
		newEnd += Vec3(.0001f, .0001f, 0.f);
		transform.Append(Mat44::MakeLookAtTransform(start, newEnd));
	}
	else {
		transform.Append(Mat44::MakeLookAtTransform(start, end));
	}
	TransformVertexArray3D(localVerts, transform);
	for (Vertex_PCU vertex : localVerts) {
		verts.push_back(vertex);
	}
}
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, int numSlice) {
	Vec3 INormal = (end - start).GetNormalized();
	AddVertsForCylinder3D(verts, start, end - INormal * 2.f*radius, radius, color, AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)), numSlice);
	AddVertsForCone3D(verts, end - INormal * 2.f*radius, end + INormal * 2.f * radius, radius*3.f, color, AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f)), numSlice);
}
void AddVertsForSphere3D(std::vector<Vertex_PCU>&verts, float radius, const Rgba8& color, const AABB2& UV, int numSlice, int numStack) {
	for (int sliceNum = 0; sliceNum < numSlice; sliceNum++) {
		float startDeg = (float)sliceNum * 360.f / numSlice;
		float endDeg = (float)(sliceNum + 1) * 360.f / numSlice;
		float degPerStack = 180.f / numStack;
		float UVX = UV.m_maxs.x - UV.m_mins.x;
		float UVY = UV.m_maxs.y - UV.m_mins.y;
		for (int stackNum = 0; stackNum < numStack; stackNum++) {
			float yMin = (UVY-((((float)stackNum) / (float)numStack) * UVY)) + UV.m_mins.y;
			float yMax = (UVY-(((float)stackNum + 1.f) / (float)numStack) * UVY) + UV.m_mins.y;
			float xMin = ((((float)sliceNum) / (float)numSlice) * UVX) + UV.m_mins.x;
			float xMax = ((((float)sliceNum + 1.f) / (float)numSlice) * UVX) + UV.m_mins.x;
			if (stackNum == 0) {
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f, radius), color,       Vec2(xMin, yMin)));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(endDeg, (stackNum + 1) * degPerStack - 90.f, radius), color,   Vec2(xMax, yMax)));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f, radius), color, Vec2(xMin, yMax)));

			}
			else if (stackNum == numStack-1) {
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f, radius), color,       Vec2(xMin, yMin)));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(endDeg, stackNum * degPerStack - 90.f, radius), color,         Vec2(xMax, yMin)));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f, radius), color, Vec2(xMax, yMax)));
			}
			else {
				
				AABB2 newUV = AABB2(Vec2(xMax, yMin), Vec2(xMin, yMax));
				AddVertsForQuad3D(verts,
					Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f, radius),
					Vec3::MakeFromPolarDegree(endDeg, stackNum * degPerStack - 90.f, radius),
					Vec3::MakeFromPolarDegree(endDeg, (stackNum + 1) * degPerStack - 90.f, radius),
					Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f, radius),
					color, newUV);
			}
		}
	}

}

void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts,
	const AABB3& cube,
	const Rgba8& color,
	const AABB2& UV) {
	Vec3 mins = cube.m_mins;
	Vec3 maxs = cube.m_maxs;
	Vec3 x_min_y_min_z_max(mins.x, mins.y, maxs.z);
	Vec3 x_min_y_max_z_min(mins.x, maxs.y, mins.z);
	Vec3 x_min_y_max_z_max(mins.x, maxs.y, maxs.z);
	Vec3 x_max_y_min_z_min(maxs.x, mins.y, mins.z);
	Vec3 x_max_y_min_z_max(maxs.x, mins.y, maxs.z);
	Vec3 x_max_y_max_z_min(maxs.x, maxs.y, mins.z);

	AddVertsForQuad3D(verts, mins, x_min_y_max_z_min, x_min_y_max_z_max, x_min_y_min_z_max, color, UV);
	AddVertsForQuad3D(verts, x_max_y_max_z_min, x_max_y_min_z_min, x_max_y_min_z_max, maxs, color, UV);
	AddVertsForQuad3D(verts, x_max_y_min_z_min, mins, x_min_y_min_z_max, x_max_y_min_z_max, color, UV);
	AddVertsForQuad3D(verts, x_min_y_max_z_min, x_max_y_max_z_min, maxs, x_min_y_max_z_max, color, UV);

	AddVertsForQuad3D(verts, mins, x_max_y_min_z_min, x_max_y_max_z_min, x_min_y_max_z_min, color, UV);
	AddVertsForQuad3D(verts, x_max_y_min_z_max, x_min_y_min_z_max, x_min_y_max_z_max, maxs, color, UV);
}

void AddVertsForZCylinder3D(std::vector<Vertex_PCU>& verts,
	const Vec3& bottom, float height, float radius,
	const Rgba8& color,
	const AABB2& UV, int numSlice) {
	AddVertsForCylinder3D(verts,bottom,Vec3(bottom.x,bottom.y, bottom.z+height),radius,color,UV,numSlice);
}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color, const AABB2& UV) {

	Vec3 i = bottomRight - bottomLeft;
	Vec3 j = topLeft - bottomLeft;
	Vec3 normal = CrossProduct3D(i, j).GetNormalized();
	Vertex_PCU const& BLV = Vertex_PCU(Vec3(bottomLeft.x, bottomLeft.y, bottomLeft.z), color, Vec2(UV.m_maxs.x, UV.m_mins.y));
	Vertex_PCU const& BRV = Vertex_PCU(Vec3(bottomRight.x, bottomRight.y, bottomRight.z), color, Vec2(UV.m_mins.x, UV.m_mins.y));
	Vertex_PCU const& TRV = Vertex_PCU(Vec3(topRight.x, topRight.y, topRight.z), color, Vec2(UV.m_mins.x, UV.m_maxs.y));
	Vertex_PCU const& TLV = Vertex_PCU(Vec3(topLeft.x, topLeft.y, topLeft.z), color, Vec2(UV.m_maxs.x, UV.m_maxs.y));
	unsigned int size = (unsigned int)verts.size();
	verts.push_back(BLV);
	verts.push_back(BRV);
	verts.push_back(TRV);
	verts.push_back(TLV);
	indexes.push_back(size);
	indexes.push_back(size + 1);
	indexes.push_back(size + 2);
	indexes.push_back(size);
	indexes.push_back(size + 2);
	indexes.push_back(size + 3);

}

void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color, const AABB2& UV) {

	Vec3 i = bottomRight-bottomLeft;
	Vec3 j = topLeft-bottomLeft;
	Vec3 normal = -CrossProduct3D(i,j).GetNormalized();
	Vec3 tangent = i.GetNormalized();
	Vec3 bitangent = j.GetNormalized();
	Vertex_PCUTBN const& BLV = Vertex_PCUTBN(Vec3(bottomLeft.x, bottomLeft.y, bottomLeft.z), color, Vec2(UV.m_maxs.x, UV.m_mins.y),tangent, bitangent, normal);
	Vertex_PCUTBN const& BRV = Vertex_PCUTBN(Vec3(bottomRight.x, bottomRight.y, bottomRight.z), color, Vec2(UV.m_mins.x, UV.m_mins.y), tangent, bitangent, normal);
	Vertex_PCUTBN const& TRV = Vertex_PCUTBN(Vec3(topRight.x, topRight.y, topRight.z), color, Vec2(UV.m_mins.x, UV.m_maxs.y), tangent, bitangent, normal);
	Vertex_PCUTBN const& TLV = Vertex_PCUTBN(Vec3(topLeft.x, topLeft.y, topLeft.z), color, Vec2(UV.m_maxs.x, UV.m_maxs.y), tangent, bitangent, normal);
	unsigned int size = (unsigned int)verts.size();
	verts.push_back(BLV);
	verts.push_back(BRV);
	verts.push_back(TRV);
	verts.push_back(TLV);
	indexes.push_back(size);
	indexes.push_back(size+1);
	indexes.push_back(size+2);
	indexes.push_back(size);
	indexes.push_back(size+2);
	indexes.push_back(size+3);

}
void AddVertsForCubicBeizer(std::vector<Vertex_PCU>& verts, CubicBeizerCurve2D curve, Rgba8 color, int numSplit) {
	for (int i = 0; i < numSplit; i++) {
		Vec2 currentPos = curve.EvaluateAtParametric((float)i / (float)numSplit);
		Vec2 nextPos = curve.EvaluateAtParametric((float)(i + 1) / (float)numSplit);
		AddVertsForLine(verts, currentPos, nextPos, color, Vec2(0, 0), .5f);
	}
}

void AddVertsForOBB3D(std::vector<Vertex_PCU>& verts, const OBB3& cube, const Rgba8& color, const AABB2& UV) {
	AddVertsForAABB3D(verts, cube.GetAsLocalAABB3(),color,UV);
	TransformVertexArray3D(verts, cube.GetTransformMatrix());
}


void AddVertsForPlane3D(std::vector<Vertex_PCU>& verts, const Plane3& plane) {
	Vec3 jNormal = CrossProduct3D( Vec3(0.f, 0.f, 1.f), plane.m_normal).GetNormalized();
	Vec3 kNormal = CrossProduct3D(plane.m_normal, jNormal).GetNormalized();
	Vec3 center = plane.m_normal * plane.m_dist;
	//render horizontal lines
	for (int j = -100; j <= 100; j++) {
		AddVertsForCylinder3D(verts, center + (float)j * jNormal + (-100.f * kNormal), center + (float)j * jNormal + 100.f * kNormal, .05f, Rgba8::GREEN, AABB2::DEFAULT);
	}
	//render vertical lines
	for (int k = -100; k <= 100; k++) {
		AddVertsForCylinder3D(verts, center + (float)k * kNormal + (-100.f * jNormal), center + (float)k * kNormal + 100.f * jNormal, .05f, Rgba8::RED, AABB2::DEFAULT);
	}
}

//Vert CPUTBN--------------------------------------------------------

void AddVertsForSphere3D(std::vector<Vertex_PCUTBN>& verts, 
std::vector<unsigned int>& indexes, 
Vec3 center,float radius, const Rgba8& color, const AABB2& UV, int numSlice, int numStack) {
	for (int sliceNum = 0; sliceNum < numSlice; sliceNum++) {
		float startDeg = (float)sliceNum * 360.f / numSlice;
		float endDeg = (float)(sliceNum + 1) * 360.f / numSlice;
		float degPerStack = 180.f / numStack;
		float UVX = UV.m_maxs.x - UV.m_mins.x;
		float UVY = UV.m_maxs.y - UV.m_mins.y;
		for (int stackNum = 0; stackNum < numStack; stackNum++) {
			float yMin = (UVY - ((((float)stackNum) / (float)numStack) * UVY)) + UV.m_mins.y;
			float yMax = (UVY - (((float)stackNum + 1.f) / (float)numStack) * UVY) + UV.m_mins.y;
			float xMin = ((((float)sliceNum) / (float)numSlice) * UVX) + UV.m_mins.x;
			float xMax = ((((float)sliceNum + 1.f) / (float)numSlice) * UVX) + UV.m_mins.x;
			if (stackNum == 0) {
				unsigned int size = (unsigned int)verts.size();
				Vec3 tangent = Vec3(1.f,0.f,0.f);
				Vec3 bitangent = Vec3(0.f,1.f,0.f);
				Vec3 normal = Vec3(0.f,0.f, 1.f);
				verts.push_back(Vertex_PCUTBN(Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f, radius)+center, color, Vec2(xMin, yMin),       tangent, bitangent, normal));
				verts.push_back(Vertex_PCUTBN(Vec3::MakeFromPolarDegree(endDeg, (stackNum + 1) * degPerStack - 90.f, radius)+center, color, Vec2(xMax, yMax),   tangent, bitangent, normal));
				verts.push_back(Vertex_PCUTBN(Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f, radius)+center, color, Vec2(xMin, yMax), tangent, bitangent, normal));
				indexes.push_back(size);
				indexes.push_back(size + 1);
				indexes.push_back(size + 2);

			}
			else if (stackNum == numStack - 1) {
				unsigned int size = (unsigned int)verts.size();
				Vec3 tangent = Vec3(1.f, 0.f, 0.f);
				Vec3 bitangent = Vec3(0.f, 1.f, 0.f);
				Vec3 normal = Vec3(0.f, 0.f, 1.f);
				verts.push_back(Vertex_PCUTBN(Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f, radius)+center, color, Vec2(xMin, yMin),       tangent, bitangent, normal));
				verts.push_back(Vertex_PCUTBN(Vec3::MakeFromPolarDegree(endDeg, stackNum * degPerStack - 90.f, radius)+center, color, Vec2(xMax, yMin),         tangent, bitangent, normal));
				verts.push_back(Vertex_PCUTBN(Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f, radius)+center, color, Vec2(xMax, yMax), tangent, bitangent, normal));
				indexes.push_back(size);
				indexes.push_back(size + 1);
				indexes.push_back(size + 2);
			}
			else {

				AABB2 newUV = AABB2(Vec2(xMax, yMin), Vec2(xMin, yMax));
				AddVertsForQuad3D(verts, indexes,
					Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f, radius)       + center,
					Vec3::MakeFromPolarDegree(endDeg, stackNum * degPerStack - 90.f, radius)         + center,
					Vec3::MakeFromPolarDegree(endDeg, (stackNum + 1) * degPerStack - 90.f, radius)   + center,
					Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f, radius) + center,
					color, newUV);
			}
		}
	}
	for (Vertex_PCUTBN& vert : verts) {
		vert.m_normal = (vert.m_position-center).GetNormalized();
		vert.m_tangent=-CrossProduct3D(vert.m_normal, Vec3(0.f,0.f,1.f)).GetNormalized();
		vert.m_bitTangent = CrossProduct3D(vert.m_normal, vert.m_tangent).GetNormalized();
	}
}

void AddVertsForAABB3D(std::vector<Vertex_PCUTBN>& verts,
    std::vector<unsigned int>& indexes,
	const AABB3& cube,
	const Rgba8& color,
	const AABB2& UV) {
	Vec3 mins = cube.m_mins;
	Vec3 maxs = cube.m_maxs;
	Vec3 x_min_y_min_z_max(mins.x, mins.y, maxs.z);
	Vec3 x_min_y_max_z_min(mins.x, maxs.y, mins.z);
	Vec3 x_min_y_max_z_max(mins.x, maxs.y, maxs.z);
	Vec3 x_max_y_min_z_min(maxs.x, mins.y, mins.z);
	Vec3 x_max_y_min_z_max(maxs.x, mins.y, maxs.z);
	Vec3 x_max_y_max_z_min(maxs.x, maxs.y, mins.z);

	AddVertsForQuad3D(verts, indexes, mins, x_min_y_max_z_min, x_min_y_max_z_max, x_min_y_min_z_max, color, UV);
	AddVertsForQuad3D(verts, indexes, x_max_y_max_z_min, x_max_y_min_z_min, x_max_y_min_z_max, maxs, color, UV);
	AddVertsForQuad3D(verts, indexes, x_max_y_min_z_min, mins, x_min_y_min_z_max, x_max_y_min_z_max, color, UV);
	AddVertsForQuad3D(verts, indexes, x_min_y_max_z_min, x_max_y_max_z_min, maxs, x_min_y_max_z_max, color, UV);

	AddVertsForQuad3D(verts, indexes, mins, x_max_y_min_z_min, x_max_y_max_z_min, x_min_y_max_z_min, color, UV);
	AddVertsForQuad3D(verts, indexes, x_max_y_min_z_max, x_min_y_min_z_max, x_min_y_max_z_max, maxs, color, UV);
}

void AddVertsForZCylinder3D(std::vector<Vertex_PCUTBN>& verts,
std::vector<unsigned int>& indexes, 
const Vec3& start, float height, float radius, const Rgba8& color, const AABB2& UV, int numSlice) {
	float UVX = UV.m_maxs.x - UV.m_mins.x;
	float UVY = UV.m_maxs.y - UV.m_mins.y;
	float degPerSlice = 360.f / numSlice;
	std::vector<Vertex_PCUTBN>localVerts;
	std::vector<unsigned int>localIndexes;
	float len = height;
	Vec3 endstart(0.f, 0.f, 1.f);
	Vec3 startend (0.f, 0.f, -1.f);
	for (int i = 0; i < numSlice; i++) {

		Vec3 BR = Vec3(0.f, CosDegrees(degPerSlice * i) * radius, SinDegrees(degPerSlice * i) * radius);
		Vec3 BL = Vec3(0.f, CosDegrees(degPerSlice * (i + 1)) * radius, SinDegrees(degPerSlice * (i + 1)) * radius);
		Vec3 TR = Vec3(len, 0.f, 0.f) + Vec3(0.f, CosDegrees(degPerSlice * i) * radius, SinDegrees(degPerSlice * i) * radius);
		Vec3 TL = Vec3(len, 0.f, 0.f) + Vec3(0.f, CosDegrees(degPerSlice * (i + 1.f)) * radius, SinDegrees(degPerSlice * (i + 1.f)) * radius);
		
		//add bottom triangle
		unsigned int size = (unsigned int)localVerts.size();

		localIndexes.push_back(size);
		localIndexes.push_back(size+1);
		localIndexes.push_back(size+2);

		localVerts.push_back(Vertex_PCUTBN(Vec3(0.f, 0.f, 0.f), color, Vec2(UVX / 2.f + UV.m_mins.x, UVY / 2.f + UV.m_mins.y), Vec3(),Vec3(),endstart));
		localVerts.push_back(Vertex_PCUTBN(BR, color, Vec2(CosDegrees(degPerSlice * (float)i) * UVX / 2 + UVX / 2, SinDegrees(degPerSlice * (float)i) * UVY / 2 + UVY / 2), Vec3(), Vec3(), endstart));
		localVerts.push_back(Vertex_PCUTBN(BL, color, Vec2(CosDegrees(degPerSlice * ((float)i + 1.f)) * UVX / 2 + UVX / 2, SinDegrees(degPerSlice * (float)(i + 1.f)) * UVY / 2 + UVY / 2), Vec3(), Vec3(), endstart));

		//add top triangle
		size = (unsigned int)localVerts.size();
		localIndexes.push_back(size);
		localIndexes.push_back(size + 1);
		localIndexes.push_back(size+2);

		localVerts.push_back(Vertex_PCUTBN(Vec3(len, 0.f, 0.f), color, Vec2(UVX / 2.f + UV.m_mins.x, UVY / 2.f), Vec3(), Vec3(), startend));
		localVerts.push_back(Vertex_PCUTBN(TL, color, Vec2(CosDegrees(degPerSlice * ((float)i + 1.f)) * UVX / 2 + UVX / 2, SinDegrees(degPerSlice * (float)(i + 1.f)) * UVY / 2 + UVY / 2), Vec3(), Vec3(), endstart));
		localVerts.push_back(Vertex_PCUTBN(TR, color, Vec2(CosDegrees(degPerSlice * (float)i) * UVX / 2 + UVX / 2, SinDegrees(degPerSlice * (float)i) * UVY / 2 + UVY / 2), Vec3(), Vec3(), endstart));
		//add side
		
		AddVertsForQuad3D(localVerts, localIndexes, BL, BR, TR, TL, color, AABB2(Vec2(((float)i / (float)numSlice) * UVX + UV.m_mins.x, UV.m_mins.y), Vec2((((float)i + 1.f) / (float)numSlice) * UVX + UV.m_mins.x, UV.m_maxs.y)));
	}

	for (Vertex_PCUTBN& vert : localVerts) {
		if (vert.m_normal != endstart && vert.m_normal != startend) {

			vert.m_normal = (Vec3(vert.m_position.x, vert.m_position.y, 0.f) - Vec3(start.x, start.y, 0.f)).GetNormalized();
			vert.m_bitTangent = CrossProduct3D(vert.m_normal,endstart).GetNormalized();
			vert.m_tangent = startend;
		}
		else {
		    vert.m_tangent = Vec3(1.f ,0.f ,0.f);
			vert.m_bitTangent = Vec3(0.f ,1.f ,0.f);
		}
	}

	Mat44 transform = Mat44::MakeTranslation3D(start);
	Vec3 newEnd = start+Vec3(.0000001f,.0000001f,height);
	transform.Append(Mat44::MakeLookAtTransform(start, newEnd));

	TransformVertexArray3D(localVerts, transform);

	unsigned int size = (unsigned int)verts.size();
	for (unsigned int i : localIndexes) {
		indexes.push_back(i+size);
	}
	for (Vertex_PCUTBN vertex : localVerts) {
		verts.push_back(vertex);
	}
	
}


