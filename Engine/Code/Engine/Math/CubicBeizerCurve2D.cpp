#include "CubicBeizerCurve2D.hpp"

CubicBeizerCurve2D::CubicBeizerCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos)
:m_startPos(startPos)
,m_guidePos1(guidePos1)
,m_guidePos2(guidePos2)
,m_endPos(endPos)
{}
CubicBeizerCurve2D::CubicBeizerCurve2D(CubicHermiteCurve const& fromHermite)
	:m_startPos(fromHermite.m_startPos)
	, m_endPos(fromHermite.m_endPos)
	,m_guidePos1(fromHermite.m_startPos+fromHermite.m_startVelocity/3.f)
	,m_guidePos2(fromHermite.m_endPos-fromHermite.m_endVelocity/3.f)
{}
Vec2 CubicBeizerCurve2D::EvaluateAtParametric(float parametricZeroToOne)const {
	return Vec2(ComputeCubicBezier1D(m_startPos.x,m_guidePos1.x,m_guidePos2.x,m_endPos.x,parametricZeroToOne),
		ComputeCubicBezier1D(m_startPos.y, m_guidePos1.y, m_guidePos2.y, m_endPos.y, parametricZeroToOne));
}
float CubicBeizerCurve2D::GetApproximateLength(int numSplit)const {
	float length = 0;
	for (int i = 0; i < numSplit; i++) {
		Vec2 currentPos = EvaluateAtParametric((float)i / (float)numSplit);
		Vec2 nextPos = EvaluateAtParametric((float)(i + 1) / (float)numSplit);
		length += (nextPos - currentPos).GetLength();
	}
	return length;
}
Vec2 CubicBeizerCurve2D::EvaluateAtApproximateDistance(float distAlongCurve, int numSplit) const {
	float distTravelled = 0;
	for (int i = 0; i < numSplit; i++) {
		Vec2 currentPos = EvaluateAtParametric((float)i / (float)numSplit);
		Vec2 nextPos = EvaluateAtParametric((float)(i + 1) / (float)numSplit);
		float nextDistance= distTravelled + (nextPos - currentPos).GetLength();
		if (nextDistance > distAlongCurve) {
			//find pos in this segment
			float remainingLength = distAlongCurve - distTravelled;
			float segmentLength = nextDistance - distTravelled;
			float t = remainingLength / segmentLength;
			return (nextPos - currentPos) * t + currentPos;
		}
		else {
			distTravelled = nextDistance;
		}
	}
	return m_endPos;
}

std::vector<CubicBeizerCurve2D> CubicBeizerCurve2D::MakeCatmullRom(std::vector<Vec2>const& positions, std::vector<CubicHermiteCurve>& out) {
	Vec2 prevPos=positions[0];
	Vec2 PrevSpd(0, 0);
	Vec2 currentPos;
	Vec2 currentSpd;
	CubicHermiteCurve currentHermite;
	std::vector<CubicBeizerCurve2D> result;
	for (int i = 1; i < positions.size();i++) {
		currentPos = positions[i];
		if (i == positions.size()-1) {
			currentSpd=Vec2(0, 0);
		}
		else {
			currentSpd = (positions[i + 1] - prevPos) / 2;
		}
		currentHermite.m_startPos = prevPos;
		currentHermite.m_startVelocity = PrevSpd;
		currentHermite.m_endPos = currentPos;
		currentHermite.m_endVelocity = currentSpd;
		result.push_back(CubicBeizerCurve2D(currentHermite));
		out.push_back(currentHermite);
		prevPos = currentPos;
		PrevSpd = currentSpd;
	}
	return result;

}