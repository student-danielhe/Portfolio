#pragma once
#include "GameCommon.hpp"
#include "Cube.hpp"
#include "Faction.hpp"
#include <vector>
#include "Engine/Core/Timer.hpp"
#include "VFX.hpp"
class Game;
struct DescriptionUI {
    AABB2 MainBody = AABB2(Vec2(10.f,25.f), Vec2(70.f, 40.f));
    AABB2 MainStatsBox = AABB2(Vec2(11.f, 26.f), Vec2(22.f, 39.f));
    AABB2 HealthBox = AABB2(Vec2(13.f, 32.f), Vec2(20.f,34.f));
    AABB2 DescriptionBox = AABB2(Vec2(23.f, 26.f), Vec2(57.f, 39.f));
    AABB2 ImageBox = AABB2(Vec2(58.5f, 28.f), Vec2(68.5f, 38.f));
};
class Map {
public:
    std::vector<Cube*> m_cubesOnMap;
    Faction* m_ally = nullptr;
    Faction* m_enemy = nullptr;
    Game* m_owner = nullptr;
    Timer* m_PhysicsTimer=nullptr;
    DescriptionUI* m_descriptionUI =nullptr;

    Cube* m_reservedForUI = nullptr;

    bool m_debugging = false;
    std::vector<VFX*> m_VFXs;

    Map(Game* owner);
    void MoveCube(Cube* cube, IntVec2 direction);
    bool IsPositionInBound(IntVec2 pos);
    
    int PosToIndex(IntVec2 pos);
    IntVec2 IndexToPos(int index);
    Vec2 GridToScreen(IntVec2 gridPos);
    void Update(float deltaSecond);
    void PhysicsUpdate();
    void CleanUpCubeAndVFX();
    void Render();
    void StartUp();
    void InitializeCubesOnMap();
    bool SpawnCube(CubeType type, IntVec2 pos, FactionType faction = FactionType::NEUTRAL);
    bool SpawnCube(CubeDefinition const& def, IntVec2 pos, FactionType faction = FactionType::NEUTRAL);
    void HandleKeyPressed();
    IntVec2 GetMouseCell();
    void RenderPreview();
    void RenderTime();
    void RenderDescription(Cube* cube);
};