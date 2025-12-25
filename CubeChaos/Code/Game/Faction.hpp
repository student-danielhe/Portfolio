#pragma once
#include "GameCommon.hpp"
#include "FactionUI.hpp"
#include <vector>
class Map;
class Cube;

class Faction {
public:
	FactionUI* m_UI = nullptr;
	FactionType m_factionID = FactionType::NEUTRAL;
	std::vector<Cube*> m_cubesOnMap;
	Map* m_owner = nullptr;
	Faction(FactionType type, Map* owner);
};