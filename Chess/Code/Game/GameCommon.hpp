#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "ChessPieceDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
class App;
extern App* g_theApp;
class Camera;
extern Camera* g_thePlayerCamera;
class Renderer;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern RandomNumberGenerator* g_RNG;
class BitmapFont;
extern BitmapFont* g_font;
constexpr float WORLD_SIZE_X = 400.f;
constexpr float WORLD_SIZE_Y = 200.f;
constexpr float CellSize = 1.f;
class ChessMatch;
extern ChessMatch* g_theMatch;
class Clock;
extern Clock* g_gameClock;
class Player;
extern Player* g_player;
class NetworkSystem;
extern NetworkSystem* g_network;
class Game;
extern Game* g_game;
void DebugDrawRing(Vec2 pos, float radius, float thickness, Rgba8 color);
void DebugDrawLine(Vec2 start, Vec2 end,float thickness, Rgba8 color);
int GetConnectedControllerID();

