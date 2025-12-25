#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
class App;
extern App* g_theApp;
class Renderer;
extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
constexpr int NUM_STARTING_ASTEROIDS = 6;
constexpr int MAX_ASTEROIDS = 12;
constexpr int MAX_BULLETS = 500;
constexpr int MAX_BEETLE= 15;
constexpr int MAX_WASP = 10;
constexpr int MAX_DEBRIS = 5000;
constexpr float WORLD_SIZE_X = 1600.f;
constexpr float WORLD_SIZE_Y = 800.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float ASTEROID_SPEED = 10.f * WORLD_SIZE_X / 400.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 12.0f*WORLD_SIZE_X/400.f;
constexpr float ASTEROID_COSMETIC_RADIUS = 15.0f * WORLD_SIZE_X / 400.f;
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f * WORLD_SIZE_X / 400.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f * WORLD_SIZE_X / 400.f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f * WORLD_SIZE_X / 400.f;
constexpr float PLAYER_SHIP_ACCELERATION = 30.f * WORLD_SIZE_X / 400.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f * WORLD_SIZE_X / 400.f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f * WORLD_SIZE_X / 400.f;
constexpr float BEETLE_MAXHP = 30.0f;
constexpr float WASP_MAXHP = 50.0f;
constexpr float ASTEROID_MAXHP = 150.0f;

void DebugDrawRing(Vec2 pos, float radius, float thickness, Rgba8 color);
void DebugDrawLine(Vec2 start, Vec2 end,float thickness, Rgba8 color);
void DrawHP(Vec2 start, Vec2 end, float thickness, Rgba8 color);
int GetConnectedControllerID();

