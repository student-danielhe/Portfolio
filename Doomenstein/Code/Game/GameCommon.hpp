#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "ActorHandle.hpp"
#include "Engine/Core/Clock.hpp"
class App;
extern App* g_theApp;
class PlayerController;
extern PlayerController* g_player1Controller;
class Camera;
extern Camera* g_screenCamera;
class Renderer;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern RandomNumberGenerator* g_RNG;
extern Clock* g_gameClock;
class BitmapFont;
extern BitmapFont* g_squirrelFont;
constexpr float WORLD_SIZE_X = 400.f;
constexpr float WORLD_SIZE_Y = 200.f;
constexpr float SCREEN_SIZE_X = 48.f;
constexpr float SCREEN_SIZE_Y = 24.f;
constexpr float PHYSICS_DELTA_SECOND = 1 / 60.f;

class Game;
extern Game* g_theGame;
void DebugDrawRing(Vec2 pos, float radius, float thickness, Rgba8 color);
void DebugDrawLine(Vec2 start, Vec2 end,float thickness, Rgba8 color);
int GetConnectedControllerID();

