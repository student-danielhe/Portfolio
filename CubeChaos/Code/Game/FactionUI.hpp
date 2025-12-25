#pragma once
#include "GameCommon.hpp"
#include <vector>
#include "CubeDefinition.hpp"
#include "Engine/Core/Timer.hpp"
class Cube;
class FactionUI {
public:
    std::vector<CubeDefinition*> m_cubeList;
    int m_mana=50;
    int m_maxMana=100;
    Timer* m_manaTimer=nullptr;
    FactionType m_factionID=FactionType::NEUTRAL;
    int m_selectedCube=0;
    Cube* m_leader = nullptr;
    //Cube* m_leader=nullptr;
    void Render();
    FactionUI(FactionType factionID);
    void AddCube(CubeDefinition* def);
    void Update();
    void HandleKeyPressed();
    void EnemyPlaceCube();
    IntVec2 GetValidAIPlacement();
    bool AICheckThisPosition(IntVec2 position);
    bool OppositeFaction(Cube* cube);

    CubeDefinition* GetMouseOver();
    int GetMouseOverIndex();
};