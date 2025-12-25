#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/RawNoise.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Core/TileHeatMap.hpp"
class App;
extern App* g_theApp;
//class Clock;
extern Clock* g_gameClock;
class Renderer;
extern Renderer* g_theRenderer;
class Game;
extern Game* g_game;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
//extern RandomNumberGenerator* g_RNG;
extern BitmapFont* g_squirrelFont;

constexpr float SCREEN_SIZE_X = 100.f;
constexpr float SCREEN_SIZE_Y = 50.f;

constexpr float MOVEMENT_TIME = .16f;


extern int GAME_SEED;
constexpr float DEFAULT_OCTAVE_PERSISTANCE = 0.5f;
constexpr float DEFAULT_NOISE_OCTAVE_SCALE = 2.0f;

//Gameplay Object Spawn Bias---------------------------
extern float ENEMYSPAWN_MAINPATH_BIAS;
extern float ENEMYSPAWN_SPLITPATH_BIAS;
extern float ENEMYSPAWN_ENEMY_BIAS;
extern float ENEMYSPAWN_LOOT_BIAS;
extern float ENEMYSPAWN_OPENESS_BIAS;
extern int ENEMY_APART;

extern float LOOTSPAWN_MAINPATH_BIAS;
extern float LOOTSPAWN_SPLITPATH_BIAS;
extern float LOOTSPAWN_LOOT_BIAS;
extern float LOOTSPAWN_OPENESS_BIAS;
extern int LOOT_APART;

void DebugDrawRing(Vec2 pos, float radius, float thickness, Rgba8 color);
void DebugDrawLine(Vec2 start, Vec2 end,float thickness, Rgba8 color);
int GetConnectedControllerID();

int Noise1DGetRandomIntInRange(int input, int minInclusive, int maxInclusive);
int Noise2DGetRandomIntInRange(int inputX,int inputY, int minInclusive, int maxInclusive);
int Noise4DGetRandomIntInRange(int inputX,int inputY, int inputZ, int inputT, int minInclusive, int maxInclusive);

void CellularAutomataGrid(std::vector<bool>& grids, IntVec2 dimension, std::vector<int> born, std::vector<int> survive, int maxIteration = 20);
int GetNumLiveNeighbor(std::vector<bool> grid, IntVec2 dimension, int index);

bool InBound(IntVec2 dimensions, IntVec2 pos);

std::vector<IntVec2> SpreadHeat(TileHeatMap& heatMap, std::vector<IntVec2> previousTiles, int currentHeat, std::vector<bool> spreadAble);

bool IsSpreadable(TileHeatMap& map, IntVec2 tile, std::vector<bool> spreadAble);

int GetRandomIndexFromWeightedList(std::vector<float> list, int inputX,int inputY, int inputZ, int inputT);