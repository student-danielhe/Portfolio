#pragma once
#include "Entity.hpp"
class Scorpio : public Entity {
	friend class Map;
protected:
	explicit Scorpio(Map* owner, Vec2 const& pos, float ordientationDegree);
	~Scorpio(){}
	virtual void Update(float deltaSecond) override;
	virtual void RenderBase(std::vector<Vertex_PCU>& verts) override;
	virtual void RenderLaser(std::vector<Vertex_PCU>& verts) override;
	bool IsPlayerInRange();
	void Shoot(float deltaSecond);
};