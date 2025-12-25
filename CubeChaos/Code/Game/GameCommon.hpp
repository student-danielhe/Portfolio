#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include <string>
#include "Engine/Core/EngineCommon.hpp"
class Game;
extern Game* g_theGame;
class App;
extern App* g_theApp;
class Camera;
extern Camera* g_ScreenCamera;
//class Renderer;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern RandomNumberGenerator* g_RNG;
class BitmapFont;
extern BitmapFont* g_font;
constexpr int ROWNUM = 28;
constexpr int COLNUM = 52;
constexpr float CUBESIZE=1.f;
constexpr float WORLDSIZEX = 80.f;
constexpr float WORLDSIZEY = 40.f;
constexpr float fixedPhysicsTime = .017f;
constexpr float UICUBESIZE = 3.f;
class Window;
extern Window* g_theWindow;

enum class FactionType {
	NEUTRAL,
	ALLY,
	ENEMY
};

void DebugDrawRing(Vec2 pos, float radius, float thickness, Rgba8 color);
void DebugDrawLine(Vec2 start, Vec2 end,float thickness, Rgba8 color);
int GetConnectedControllerID();

