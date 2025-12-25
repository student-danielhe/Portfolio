#pragma once
#include "Tile.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer//Texture.hpp"
#include "Engine//Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include"Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
//#include "Engine/Renderer/SpriteSheet.hpp"

class App;
extern App* g_theApp;
class Renderer;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern RandomNumberGenerator* g_RNG;
class Window;
extern Window* g_theWindow;
class Game;
extern Game* g_theGame;
class SpriteSheet;
extern SpriteSheet* g_terrainSheet;
extern DevConsole* g_theDevConsole;
extern float WORLD_SIZE_X;
extern float WORLD_SIZE_Y;

void DebugDrawRing(Vec2 pos, float radius, float thickness, Rgba8 color);
void DebugDrawLine(Vec2 start, Vec2 end,float thickness, Rgba8 color);
int GetConnectedControllerID();

