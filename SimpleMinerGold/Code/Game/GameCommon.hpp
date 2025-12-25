#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Job/JobSystem.hpp"
#include "Engine/Math/CurvePiecewise.hpp"
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
class SpriteSheet;
extern SpriteSheet* g_sprite;
extern JobSystem* g_jobSystem;
constexpr float PHYSICS_FRAMERATE = .016f;
constexpr int MAX_JOB_PER_FRAME = 100;

constexpr int CHUNK_BITS_X = 4;
constexpr int CHUNK_BITS_Y = 4;
constexpr int CHUNK_BITS_Z = 7;
constexpr int CHUNK_SIZE_X = 1 << CHUNK_BITS_X;
constexpr int CHUNK_SIZE_Y = 1 << CHUNK_BITS_Y;
constexpr int CHUNK_SIZE_Z = 1 << CHUNK_BITS_Z;
constexpr int CHUNK_MAX_X = CHUNK_SIZE_X - 1;
constexpr int CHUNK_MAX_Y = CHUNK_SIZE_Y - 1;
constexpr int CHUNK_MAX_Z = CHUNK_SIZE_Z - 1;
constexpr int CHUNK_MASK_X = CHUNK_MAX_X;
constexpr int CHUNK_MASK_Y = CHUNK_MAX_Y << CHUNK_BITS_X;
constexpr int CHUNK_MASK_Z = CHUNK_MAX_Z << (CHUNK_BITS_X + CHUNK_BITS_Y);
constexpr int BLOCKS_PER_CHUNK = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

constexpr int LIGHT_BITS_INDOOR = 4;
constexpr int LIGHT_BITS_OUTDOOR = 4;
constexpr int LIGHT_SIZE_INDOOR = 1<<LIGHT_BITS_INDOOR;
constexpr int LIGHT_SIZE_OUTDOOR = 1<<LIGHT_BITS_OUTDOOR;
constexpr int LIGHT_MAX_INDOOR = LIGHT_SIZE_INDOOR - 1;
constexpr int LIGHT_MAX_OUTDOOR = LIGHT_SIZE_OUTDOOR - 1;
constexpr int LIGHT_MASK_INDOOR = LIGHT_MAX_INDOOR;
constexpr int LIGHT_MASK_OUTDOOR = LIGHT_MAX_OUTDOOR<<LIGHT_BITS_INDOOR;

constexpr int BLOCK_BIT_MASK_IS_SKY         = 1;
constexpr int BLOCK_BIT_MASK_IS_LIGHT_DIRTY	= 1<<1;
constexpr int BLOCK_BIT_MASK_IS_FULL_OPAQUE	= 1<<2;
constexpr int BLOCK_BIT_MASK_IS_SOLID		= 1<<3;
constexpr int BLOCK_BIT_MASK_IS_VISIBLE		= 1<<4;


constexpr int CHUNK_ACTIVATION_RANGE = 120;
constexpr int CHUNK_DEACTIVATION_RANGE = CHUNK_ACTIVATION_RANGE + CHUNK_SIZE_X + CHUNK_SIZE_Y;

constexpr int CHUNK_ACTIVATION_RADIUS_X = 1 + (CHUNK_ACTIVATION_RANGE / CHUNK_SIZE_X);
constexpr int CHUNK_ACTIVATION_RADIUS_Y = 1 + (CHUNK_ACTIVATION_RANGE / CHUNK_SIZE_Y);
constexpr int MAX_ACTIVE_CHUNKS = (2 * CHUNK_ACTIVATION_RADIUS_X) * (2 * CHUNK_ACTIVATION_RADIUS_Y);

constexpr unsigned int GAME_SEED = 0u;
constexpr float DEFAULT_OCTAVE_PERSISTANCE = 0.5f;
constexpr float DEFAULT_NOISE_OCTAVE_SCALE = 2.f;

constexpr float DEFAULT_DENSITY_NOISE_SCALE = 64.f;
constexpr unsigned int DEFAULT_DENSITY_NOISE_OCTAVE = 4u;
constexpr float DEFAULT_TERRAIN_HEIGHT = CHUNK_SIZE_Z/2.f;
constexpr float DEFAULT_BIAS_PER_Z = 4.f/CHUNK_SIZE_Z;

constexpr float CONTINENT_NOISE_SCALE = 1024.f;
constexpr unsigned int CONTINENT_NOISE_OCTAVES = 4u;

constexpr float EROSION_NOISE_SCALE = 256.f;
constexpr unsigned int EROSION_NOISE_OCTAVES = 4u;

constexpr float HUMIDITY_NOISE_SCALE = 256.0f;
constexpr unsigned int HUMIDITY_NOISE_OCTAVES = 3u;

constexpr float TEMPERATURE_NOISE_SCALE = 256.0f;
constexpr unsigned int TEMPERATURE_NOISE_OCTAVES = 2u;

constexpr float HILLINESS_NOISE_SCALE = 256.0f;
constexpr unsigned int HILLINESS_NOISE_OCTAVES = 4u;

constexpr int MIN_DIRT_OFFSET_Z = 3;
constexpr int MAX_DIRT_OFFSET_Z = 4;
constexpr int SEA_LEVEL_Z = 56;

constexpr float COAL_CHANCE = 0.05f;
constexpr float IRON_CHANCE = 0.02f;
constexpr float GOLD_CHANCE = 0.005f;
constexpr float DIAMOND_CHANCE = 0.0001f;
constexpr int MIN_LAVA_DEPTH = 0;
constexpr int LAVA_Z = 1;
constexpr int MIN_OBSIDIAN_DEPTH = 1;
constexpr int OBSIDIAN_Z = 2;

extern CurvePiecewise* g_Continent_Height_Offset_Curve;
extern CurvePiecewise* g_Continent_Squashing_Curve;
void DebugDrawRing(Vec2 pos, float radius, float thickness, Rgba8 color);
void DebugDrawLine(Vec2 start, Vec2 end,float thickness, Rgba8 color);
int GetConnectedControllerID();
void AddVertsForWorldBasis(std::vector<Vertex_PCU>& verts, float size);

