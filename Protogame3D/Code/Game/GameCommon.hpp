#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

class App;
extern App* g_theApp;
class Game;
extern Game* g_theGame;
class Camera;
extern Camera* g_thePlayerCamera;
class Renderer;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern RandomNumberGenerator* g_RNG;
extern BitmapFont* g_font;
class DevConsole;
extern DevConsole* g_theDevConsole;
class Clock;
extern Clock* g_gameClock;
class Window;
extern Window* g_theWindow;
constexpr float WORLD_SIZE_X = 400.f;
constexpr float WORLD_SIZE_Y = 200.f;


void DebugDrawRing(Vec2 pos, float radius, float thickness, Rgba8 color);
void DebugDrawLine(Vec2 start, Vec2 end,float thickness, Rgba8 color);
int GetConnectedControllerID();

