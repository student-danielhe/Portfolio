#include "Map.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <iostream>
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/DebugProp.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "AIController.hpp"
#include "MarineAI.hpp"
extern std::vector<DebugProp*> m_debugRenderProps;
extern std::vector<DebugProp*> m_debugRenderScreenProps;

void Map::CreateTiles() {
	std::vector<Rgba8> data = m_definition->m_image.GetPixelData();
	int x = 0;
	int y = 0;
	for (Rgba8 color : data) {
		Tile newTile;
		Vec3 tileMins(x * m_tileSize.x, y * m_tileSize.y, 0);
		newTile.m_bounds = AABB3(tileMins, tileMins+m_tileSize);
		for (TileDefinition* def : TileDefinition::s_definitions) {
			if (def->m_mapDefColor == color) {
				newTile.m_tileType = def->m_type;
			}
		}
		m_tiles.push_back(newTile);
		x++;
		if (x >= m_dimensions.x) {
			x = 0;
			y++;
		}
	}
}

void Map::CreateGeometry() {
	for (Tile tile : m_tiles) {
		TileDefinition* def = TileDefinition::GetTileDef(tile.m_tileType);
		IntVec2 invalid(-1, -1);
		if (def) {
			if (def->m_cellingCoord != invalid) {
				AddGeometryForCeilling(tile.m_bounds, m_definition->m_spiteSheet->GetSpriteUVs(IntVec2(def->m_cellingCoord.x, def->m_cellingCoord.y)));
			}

			if (def->m_floorCoord != invalid) {

  				AddGeometryForFloor(tile.m_bounds, m_definition->m_spiteSheet->GetSpriteUVs(IntVec2(def->m_floorCoord.x, def->m_floorCoord.y)));
			}
			if (def->m_wallCoord != invalid) {

				AddGeometryForWall(tile.m_bounds, m_definition->m_spiteSheet->GetSpriteUVs(IntVec2(def->m_wallCoord.x, def->m_wallCoord.y)));
			}
		}
		
	}
}
void Map::AddGeometryForWall(const AABB3& bounds, const AABB2& UV) {
	
	Vec3 mins = bounds.m_mins;
	Vec3 maxs = bounds.m_maxs;
	Vec3 x_min_y_min_z_max(mins.x, mins.y, maxs.z);
	Vec3 x_min_y_max_z_min(mins.x, maxs.y, mins.z);
	Vec3 x_min_y_max_z_max(mins.x, maxs.y, maxs.z);
	Vec3 x_max_y_min_z_min(maxs.x, mins.y, mins.z);
	Vec3 x_max_y_min_z_max(maxs.x, mins.y, maxs.z);
	Vec3 x_max_y_max_z_min(maxs.x, maxs.y, mins.z);

	AddVertsForQuad3D(m_vertexes, m_indexes, mins, x_min_y_max_z_min, x_min_y_max_z_max, x_min_y_min_z_max, Rgba8::WHITE, UV);
	AddVertsForQuad3D(m_vertexes, m_indexes, x_max_y_max_z_min, x_max_y_min_z_min, x_max_y_min_z_max, maxs, Rgba8::WHITE, UV);
	AddVertsForQuad3D(m_vertexes, m_indexes, x_max_y_min_z_min, mins, x_min_y_min_z_max, x_max_y_min_z_max, Rgba8::WHITE, UV);
	AddVertsForQuad3D(m_vertexes, m_indexes, x_min_y_max_z_min, x_max_y_max_z_min, maxs, x_min_y_max_z_max, Rgba8::WHITE, UV);

	
	
}
void Map::AddGeometryForFloor(const AABB3& bounds, const AABB2& UV) {
	Vec3 mins = bounds.m_mins;
	Vec3 maxs = bounds.m_maxs;
	Vec3 x_min_y_min_z_max(mins.x, mins.y, maxs.z);
	Vec3 x_min_y_max_z_min(mins.x, maxs.y, mins.z);
	Vec3 x_min_y_max_z_max(mins.x, maxs.y, maxs.z);
	Vec3 x_max_y_min_z_min(maxs.x, mins.y, mins.z);
	Vec3 x_max_y_min_z_max(maxs.x, mins.y, maxs.z);
	Vec3 x_max_y_max_z_min(maxs.x, maxs.y, mins.z);
	AddVertsForQuad3D(m_vertexes, m_indexes, mins, x_min_y_max_z_min,  x_max_y_max_z_min, x_max_y_min_z_min, Rgba8::WHITE, UV);
}
void Map::AddGeometryForCeilling(const AABB3& bounds, const AABB2& UV) {
	Vec3 mins = bounds.m_mins;
	Vec3 maxs = bounds.m_maxs;
	Vec3 x_min_y_min_z_max(mins.x, mins.y, maxs.z);
	Vec3 x_min_y_max_z_min(mins.x, maxs.y, mins.z);
	Vec3 x_min_y_max_z_max(mins.x, maxs.y, maxs.z);
	Vec3 x_max_y_min_z_min(maxs.x, mins.y, mins.z);
	Vec3 x_max_y_min_z_max(maxs.x, mins.y, maxs.z);
	Vec3 x_max_y_max_z_min(maxs.x, maxs.y, mins.z);
	AddVertsForQuad3D(m_vertexes, m_indexes, x_max_y_min_z_max, maxs,  x_min_y_max_z_max, x_min_y_min_z_max, Rgba8::WHITE, UV);
}

void Map::CreateBuffers() {
	m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
	m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
}

Map::Map(Game* game, const MapDefinition* defintion)
:m_game(game)
,m_definition(defintion){
	m_shader = m_definition->m_shader;
	m_texture = m_definition->m_spriteSheetTexture;
	m_dimensions = m_definition->m_image.GetDimensions();
}
Map::~Map() {

	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;
	delete m_indexBuffer;
	m_indexBuffer = nullptr;
	g_theRenderer->SetBuffers();
	delete g_player1Controller;
	g_player1Controller = nullptr;
	delete g_player2Controller;
	g_player2Controller = nullptr;
	m_actors.clear();
	g_theAudio->StopSound(m_gameMusicPlayback);
}

void Map::StartUp() {

	g_theAudio->SetNumListeners(1);
	g_player1Controller = new PlayerController(nullptr, this);
	g_player1Controller->m_playerIndex = 0;
	g_player1Controller->InitializeCameras();
	
	m_physicsTimer = new Timer(PHYSICS_DELTA_SECOND, g_gameClock);
	m_physicsTimer->Start();
	//---------------------------------------------------------------
	CreateBuffers();
	g_theRenderer->SetBuffers(m_vertexBuffer, m_indexBuffer);
	CreateTiles();
	CreateGeometry();
	//CreateActors();
	m_lightConstants = new LightConstants();
	DebugRenderConfig config;
	config.m_renderer = g_theRenderer;
	DebugRenderSystemStartUp(config);
	SubscribeEventCallbackFunction("DEBUG CLEAR", Command_DebugRenderClear);
	SubscribeEventCallbackFunction("DEBUG TOGGLE", Command_DebugRenderToggle);
	SpawnPlayer();
	for (SpawnInfo info : m_definition->m_spawnInfos) {
		SpawnInitialActor(info);
	}
	m_gameMusic = g_theAudio->CreateOrGetSound("Data/Audio/Music/E1M1_AtDoomsGate.mp2");
	m_gameMusicPlayback = g_theAudio->StartSound(m_gameMusic, true);
	//possess after actors created
	g_player1Controller->PlayerPossess(m_player1Handle);
	if (m_splitScreen) {
		g_player2Controller->PlayerPossess(m_player2Handle);
	}
	for (int i = 0; i < m_actors.size(); i++) {
		Actor* actor = m_actors[i];
		if (actor) {
			if (actor->m_definition->m_ai->m_aiEnabled) {
				if (actor->m_definition->m_base->m_name.compare("Demon") == 0) {
					DemonAIController* controller = new DemonAIController(actor->m_handle, this);
					controller->Possess(actor->m_handle);
					actor->m_AIController = controller;
				}
				else if (actor->m_definition->m_base->m_name.compare("Marine") == 0) {
					MarineAI* controller = new MarineAI(actor->m_handle, this);
					controller->Possess(actor->m_handle);
					actor->m_MarineAI = controller;
				}
			}
		}
	}
}

void Map::Update(float deltaSeconds) {
	while (m_physicsTimer->DecrementPeriodIfElapsed()) {
		g_player1Controller->UpdateInput(deltaSeconds);

		/*if(g_player2Controller)
			g_player2Controller->UpdateInput(deltaSeconds);
		*/
		DemonAIController::UpdateParty();
		for (int i = 0; i < m_actors.size(); i++) {
			Actor* actor = m_actors[i];
			if (actor && actor->m_handle->IsValid()) {
				actor->UpdatePhysics(deltaSeconds);
				if (actor->m_AIController && actor->m_controller != g_player1Controller && actor->m_controller != g_player2Controller) {
					actor->m_AIController->Update();
				}
				if (actor->m_MarineAI && actor->m_controller != g_player1Controller && actor->m_controller != g_player2Controller) {
					actor->m_MarineAI->Update();
				}
			}
		}
		CollideAllActors();
		CollideActorsWithMap();
		HandleKeyPress();
		DebugUpdate(deltaSeconds);
		AddDebugMessages();

		//-clamp light constants
		if (m_lightConstants->SunIntensity < 0.f) {
			m_lightConstants->SunIntensity = 0.f;
		}
		if (m_lightConstants->SunIntensity > 1.f) {
			m_lightConstants->SunIntensity = 1.f;
		}
		if (m_lightConstants->AmbientIntensity < 0.f) {
			m_lightConstants->AmbientIntensity = 0.f;
		}
		if (m_lightConstants->AmbientIntensity > 1.f) {
			m_lightConstants->AmbientIntensity = 1.f;
		}
		DeleteDestroyedActors();
	}
	
}
void Map::Render() {
	if (m_splitScreen) {
		RenderPlayer(g_player1Controller);
		RenderPlayer(g_player2Controller);
	}
	else {
		RenderPlayer(g_player1Controller);
	}
	DebugRenderScreen(*g_screenCamera);
	
}

void Map::RenderPlayer(PlayerController* player) {
	g_theRenderer->SetModelConstants();
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetModelConstants();

	g_theRenderer->SetLightConstants(m_lightConstants->SunDirection.GetNormalized(), m_lightConstants->SunIntensity, m_lightConstants->AmbientIntensity);
	g_theRenderer->BeginCamera(*player->m_worldCamera);
	g_theRenderer->BindShader(m_shader);
	g_theRenderer->BindTexture(&m_definition->m_spiteSheet->GetTexture());
	g_theRenderer->DrawVertexArray(m_vertexes, m_indexes);

	for (int i = 0; i < m_actors.size(); i++) {
		Actor* actor = m_actors[i];
		if (actor)
			actor->Render(player);
	}
	g_theRenderer->SetLightConstants(Vec3(1.f, 0.f, 0.f), 0.f, 1.f);
	g_theRenderer->EndCamera(*player->m_worldCamera);
	DebugRenderWorld(*player->m_worldCamera);
	if(player&&player->GetActor())
	    player->RenderUI(player->GetActor()->m_weapons[player->GetActor()->m_currentWeaponIndex]);
	
}

bool Map::IsPositionInBounds(const Vec3& position) const {
	IntVec2 coords = PositionToCoords(position);
	return AreCoordsInBounds(coords.x, coords.y);
}
bool Map::AreCoordsInBounds(int x, int y)const {
	return (x >= 0 && x < m_dimensions.x) && (y >= 0 && y < m_dimensions.y);
}
const Tile* Map::GetTile(int x, int y) const {
	if (x < 0 || y < 0 || x >= 32 || y >= 32) {
		return nullptr;
	}
	return &m_tiles[x+y*m_dimensions.x];
}
IntVec2 Map::PositionToCoords(const Vec3& position) const {
	int x = RoundDownToInt(position.x / (float)m_tileSize.x);
	int y = RoundDownToInt(position.y / (float)m_tileSize.y);
	return IntVec2(x, y);
}


void Map::CollideAllActors() {
	for (int i = 0; i < m_actors.size(); i++) {
		Actor* actorA = m_actors[i];
		if (actorA) {
			for (int j = 0; j < m_actors.size(); j++) {
				Actor* actorB = m_actors[j];
				if (actorB) {
					if (actorA != actorB && actorA->m_definition->m_collision->m_collideWithActor && actorB->m_definition->m_collision->m_collideWithActor) {
						CollideActors(actorA, actorB);
					}
				}
			}
		}
		
	}
}
void Map::CollideActors(Actor* actorA, Actor* actorB) {
	FloatRange rangeA(actorA->m_bottomPosition.z, actorA->m_bottomPosition.z + actorA->m_definition->m_collision->m_height);
	FloatRange rangeB(actorB->m_bottomPosition.z, actorB->m_bottomPosition.z + actorB->m_definition->m_collision->m_height);
	bool collide = DoZCylindersOverlap(actorA->m_bottomPosition, actorA->m_definition->m_collision->m_radius, rangeA,
		                               actorB->m_bottomPosition, actorB->m_definition->m_collision->m_radius, rangeB);
	//no proj vs proj collision
	if (collide) {
		if (!(actorA->m_definition->m_collision->m_dieOnCollide && actorB->m_definition->m_collision->m_dieOnCollide) && actorA->m_owner != actorB->m_handle && actorB->m_owner != actorA->m_handle)
		{
			Vec2 CenterA = actorA->m_bottomPosition.GetAsVec2();
			Vec2 CenterB = actorB->m_bottomPosition.GetAsVec2();
			bool actorACollides = (actorA->m_definition->m_collision->m_collideWithActor)&&!actorA->m_dead;
			bool actorBCollides = (actorB->m_definition->m_collision->m_collideWithActor)&&!actorB->m_dead;
			if ((actorACollides && actorBCollides)) {
				PushDiscsOutOfEachOther2D(CenterA, actorA->m_definition->m_collision->m_radius,
					CenterB, actorB->m_definition->m_collision->m_radius);
				actorA->OnCollide(*actorB);
				actorB->OnCollide(*actorA);
			}
			actorA->m_bottomPosition.x = CenterA.x;
			actorA->m_bottomPosition.y = CenterA.y;

			actorB->m_bottomPosition.x = CenterB.x;
			actorB->m_bottomPosition.y = CenterB.y;
		}
	}
}
void Map::CollideActorsWithMap() {
	for (int i = 0; i < m_actors.size(); i++) {
		Actor* actor = m_actors[i];
		if (actor) {
			if (actor->m_definition->m_collision->m_collideWithWorld)
				CollideActorWithMap(actor);
		}
	}
}
void Map::CollideActorWithMap(Actor* actor) {
	IntVec2 actorPosition = PositionToCoords(actor->m_bottomPosition);
	bool die = actor->m_definition->m_collision->m_dieOnCollide;
	std::vector<Tile> adjencentTiles(0);
	if (actorPosition.x < 32 && actorPosition.x >= 0 && actorPosition.y < 32 && actorPosition.y >= 0) {
		adjencentTiles.push_back(*GetTile(actorPosition.x, actorPosition.y));
		if (actorPosition.y < 31)
			adjencentTiles.push_back(*GetTile(actorPosition.x, actorPosition.y + 1));
		if (actorPosition.y > 0)
			adjencentTiles.push_back(*GetTile(actorPosition.x, actorPosition.y - 1));

		if (actorPosition.x < 31) {
			adjencentTiles.push_back(*GetTile(actorPosition.x + 1, actorPosition.y));
			if (actorPosition.y < 31)
				adjencentTiles.push_back(*GetTile(actorPosition.x + 1, actorPosition.y + 1));
			if (actorPosition.y > 0)
				adjencentTiles.push_back(*GetTile(actorPosition.x + 1, actorPosition.y - 1));
		}

		if (actorPosition.x > 0) {
			adjencentTiles.push_back(*GetTile(actorPosition.x - 1, actorPosition.y));
			if (actorPosition.y < 31)
				adjencentTiles.push_back(*GetTile(actorPosition.x - 1, actorPosition.y + 1));
			if (actorPosition.y > 0)
				adjencentTiles.push_back(*GetTile(actorPosition.x - 1, actorPosition.y - 1));
		}
	}
	    
 	
	Vec2 center = actor->m_bottomPosition.GetAsVec2();
	//collision with floor
	if (actor->m_bottomPosition.z < 0) {
		float ZDiff = actor->m_bottomPosition.z;
		actor->m_bottomPosition.z -= ZDiff;
		if (die) {
			actor->m_dead = true;
		}
	}
	//collision with ceiling
	if (actor->m_bottomPosition.z +actor->m_definition->m_collision->m_height> m_tileSize.z) {
		float ZDiff = actor->m_bottomPosition.z + actor->m_definition->m_collision->m_height - m_tileSize.z;
		actor->m_bottomPosition.z -= ZDiff;
		if (die) {
			actor->m_dead = true;
		}
	}
	//collision with wall
	for (Tile tile : adjencentTiles) {
		if (TileDefinition::GetTileDef(tile.m_tileType)->m_isSolid) {
			AABB2 box(tile.m_bounds.m_mins.GetAsVec2(), tile.m_bounds.m_maxs.GetAsVec2());
			Vec2 boxCenter = actor->m_bottomPosition.GetAsVec2();
			bool pushed = PushDiscOutOfAABB2D(boxCenter, actor->m_definition->m_collision->m_radius, box);
			actor->m_bottomPosition.x = boxCenter.x;
			actor->m_bottomPosition.y = boxCenter.y;
			if (pushed && die) {
				actor->m_dead = true;
			}
		}
	}
	
}

RaycastResult3D Map::RaycastAll(const Vec3& start, const Vec3& fwd, float dist, Actor* owner) const {
	RaycastResult3D horizontalResult = RayCastWorldXY(start, fwd, dist);
	RaycastResult3D verticalResult = RayCastWorldZ(start, fwd, dist);
	RaycastResult3D actorResult = RaycastWorldActors(start, fwd, dist, owner);
	RaycastResult3D finalResult;
	finalResult.m_didImpact = false;
	finalResult.m_impactDist = FLT_MAX;
	if (horizontalResult.m_didImpact) {
		finalResult = horizontalResult;
	}
	if (verticalResult.m_didImpact) {
		if (verticalResult.m_impactDist < finalResult.m_impactDist) {
			finalResult = verticalResult;
		}
	}
	if (actorResult.m_didImpact) {
		if (actorResult.m_impactDist < finalResult.m_impactDist) {
			finalResult = actorResult;
		}
	}
	if (finalResult.m_didImpact) {
		return finalResult;
	}
	else {
		finalResult.m_impactDist = 0.f;
		finalResult.m_impactNormal = Vec3(0.f, 0.f, 0.f);
		finalResult.m_impactPos = Vec3(0.f, 0.f, 0.f);
		return finalResult;
	}
}
RaycastResult3D Map::RayCastWorldXY(const Vec3& start, const Vec3& fwd, float dist)const {
	RaycastResult3D finalResult;
	finalResult.m_didImpact = false;
	finalResult.m_impactDist = FLT_MAX;

	for (Tile tile : m_tiles) {
		if (TileDefinition::GetTileDef(tile.m_tileType)->m_isSolid) {
			RaycastResult3D tileResult = RaycastVsAABB3D(start, fwd, dist, tile.m_bounds);
			if (tileResult.m_didImpact && tileResult.m_impactDist < finalResult.m_impactDist) {
				finalResult = tileResult;
			}
		}
	}
	if (finalResult.m_didImpact) {
		return finalResult;
	}
	else {
		finalResult.m_impactDist = 0.f;
		finalResult.m_impactNormal = Vec3(0.f, 0.f, 0.f);
		finalResult.m_impactPos = Vec3(0.f, 0.f, 0.f);
		return finalResult;
	}
}
RaycastResult3D Map::RayCastWorldZ(const Vec3& start, const Vec3& fwd, float dist)const {
	RaycastResult3D finalResult;
	if (start.z >= 0.f && start.z <= m_tileSize.z) {
		if (fwd.z < 0.f) {
			//raycast vs floor
			float zDist = start.z;
			if (zDist < -fwd.z * dist) {
				finalResult.m_didImpact = true;
				finalResult.m_impactNormal = Vec3(0.f, 0.f, 1.f);

				finalResult.m_impactDist = zDist / (-fwd.z);
				finalResult.m_impactPos = start + fwd * finalResult.m_impactDist;
				return finalResult;
			}
			
		}
		if (fwd.z > 0.f) {
			//raycast vs ceiling
			float zDist = m_tileSize.z - start.z;
			if (zDist < fwd.z * dist) {
				finalResult.m_didImpact = true;
				finalResult.m_impactNormal = Vec3(0.f, 0.f, -1.f);

				finalResult.m_impactDist = zDist / fwd.z;
				finalResult.m_impactPos = start + fwd * finalResult.m_impactDist;
				return finalResult;
			}
			
		}
	}

	finalResult.m_didImpact = false;
	finalResult.m_impactDist = 0.f;
	finalResult.m_impactNormal = Vec3(0.f, 0.f, 0.f);
	finalResult.m_impactPos = Vec3(0.f, 0.f, 0.f);
	return finalResult;
	
}
RaycastResult3D Map::RaycastWorldActors(const Vec3& start, const Vec3& fwd, float dist, Actor* owner) const {
	RaycastResult3D finalResult;
	finalResult.m_didImpact = false;
	finalResult.m_impactDist = FLT_MAX;
	for (int i = 0; i < m_actors.size(); i++) {
		Actor* actor = m_actors[i];
		if (actor) {
			if (actor != owner && actor->m_definition->m_collision->m_collideWithActor) {
				FloatRange range(actor->m_bottomPosition.z, actor->m_bottomPosition.z + actor->m_definition->m_collision->m_height);
				RaycastResult3D actorResult = RaycastVsZCylinder3D(start, fwd, dist, actor->m_bottomPosition, actor->m_definition->m_collision->m_radius, range);
				if (actorResult.m_didImpact && actorResult.m_impactDist < finalResult.m_impactDist) {
					finalResult = actorResult;
				}
			}
		}
	}
	if (finalResult.m_didImpact) {
		return finalResult;
	}
	else {
		finalResult.m_impactDist = 0.f;
		finalResult.m_impactNormal = Vec3(0.f, 0.f, 0.f);
		finalResult.m_impactPos = Vec3(0.f, 0.f, 0.f);
		return finalResult;
	}
}
RaycastResult3D Map::RaycastWorldActors(Actor& out, const Vec3& start, const Vec3& fwd, float dist, Actor* owner) const {
	RaycastResult3D finalResult;
	Actor* resultActor = nullptr;
	finalResult.m_didImpact = false;
	finalResult.m_impactDist = FLT_MAX;
	for (int i = 0; i < m_actors.size(); i++) {
		Actor* actor = m_actors[i];
		if (actor) {
			if (actor != owner && actor->m_definition->m_collision->m_collideWithActor) {
				FloatRange range(actor->m_bottomPosition.z, actor->m_bottomPosition.z + actor->m_definition->m_collision->m_height);
				RaycastResult3D actorResult = RaycastVsZCylinder3D(start, fwd, dist, actor->m_bottomPosition, actor->m_definition->m_collision->m_radius, range);
				if (actorResult.m_didImpact && actorResult.m_impactDist < finalResult.m_impactDist) {
					finalResult = actorResult;
					resultActor = actor;
				}
			}
		}
		
	}
	if (finalResult.m_didImpact) {
		out = *resultActor;
		return finalResult;
	}
	else {
		finalResult.m_impactDist = 0.f;
		finalResult.m_impactNormal = Vec3(0.f, 0.f, 0.f);
		finalResult.m_impactPos = Vec3(0.f, 0.f, 0.f);
		return finalResult;
	}
}
void Map::HandleKeyPress() {

	if (g_theInput->WasKeyJustReleased(KEYCODE_F2)) {
		m_lightConstants->SunDirection.x -= 1.f;
	}

	if (g_theInput->WasKeyJustReleased(KEYCODE_F3)) {
		m_lightConstants->SunDirection.x += 1.f;
	}

	if (g_theInput->WasKeyJustReleased(KEYCODE_F4)) {
		m_lightConstants->SunDirection.y -= 1.f;
	}

	if (g_theInput->WasKeyJustReleased(KEYCODE_F5)) {
		m_lightConstants->SunDirection.y += 1.f;
	}

	if (g_theInput->WasKeyJustReleased(KEYCODE_F6)) {
		m_lightConstants->SunIntensity -= .05f;
	}

	if (g_theInput->WasKeyJustReleased(KEYCODE_F7)) {
		m_lightConstants->SunIntensity += .05f;

	}

	if (g_theInput->WasKeyJustReleased(KEYCODE_F8)) {
		m_lightConstants->AmbientIntensity -= .05f;
	}

	if (g_theInput->WasKeyJustReleased(KEYCODE_F9)) {
		m_lightConstants->AmbientIntensity += .05f;
	}
}

void Map::AddDebugMessages() {
	std::string controlMode;
	Rgba8 color;
	
	Clock* theGameClock = g_gameClock;
	std::string time = std::to_string(RoundDownToInt((float)theGameClock->GetTotalSeconds()));
	std::string FPS = std::to_string(RoundDownToInt(1.f/(float)theGameClock->GetDeltaSeconds()));
	std::string timeScale = std::to_string(theGameClock->GetTimeScale());
	std::string clockInfo = "[GAME CLOCK] Time: " + time + " FPS: " +FPS + " Timescale: " +timeScale;
	DebugAddScreenText(clockInfo, AABB2(Vec2(0.5f, 23.f), Vec2(46.f, 24.f)), .3f, Vec2(1.f, 0.f), 0.f);
}

void Map::DebugUpdate(float gameDeltaSecond) {
	for (int i = 0; i < m_debugRenderProps.size(); i++) {
		DebugProp* prop = m_debugRenderProps[i];
		prop->Update(gameDeltaSecond);
		prop->m_currentDuration += gameDeltaSecond;
		if (prop->m_duration != -1.f) {
			if (prop->m_currentDuration >= prop->m_duration) {
				m_debugRenderProps.erase(m_debugRenderProps.begin() + i);
			}
		}
	}

	for (int i = 0; i < m_debugRenderScreenProps.size(); i++) {
		DebugProp* prop = m_debugRenderScreenProps[i];
		prop->Update(gameDeltaSecond);
		prop->m_currentDuration += gameDeltaSecond;
		if (prop->m_duration != -1.f) {
			if (prop->m_currentDuration >= prop->m_duration) {
				m_debugRenderScreenProps.erase(m_debugRenderScreenProps.begin() + i);
			}
		}
	}
}

Actor* Map::SpawnInitialActor(const SpawnInfo& spawnInfo) {
	if ((int)m_actors.size() < ActorHandle::MAX_ACTOR_INDEX && m_nextActorUID < ActorHandle::MAX_ACTOR_UID) {
		ActorHandle* handle = CreateValidHandle();
		std::string name = spawnInfo.m_actorName;
		ActorDefinition* def = ActorDefinition::GetActorDefinitionByName(name);
		Actor* actor = new Actor(def, handle);
		actor->m_bottomPosition = spawnInfo.m_position;
		actor->m_orientation = spawnInfo.m_orientation;
		actor->m_velocity = spawnInfo.m_velocity;

		actor->m_map = this;

		if (handle->GetIndex() == (int)m_actors.size())
			m_actors.push_back(actor);
		else {
			m_actors[handle->GetIndex()] = actor;
		}
		return actor;
	}
	return nullptr;
}
Actor* Map::GetActorByHandle(const ActorHandle& handle)const {
	if (&handle!=nullptr&&handle.IsValid()) {
		unsigned int index = handle.GetIndex();
		if (m_actors.size() > index) {
			if (m_actors[index]&&*m_actors[index]->m_handle == handle) {
				return m_actors[index];
			}
		}
	}
	return nullptr;
}
void Map::DeleteDestroyedActors() {
	for (int i = 0; i < m_actors.size(); i++) {
		if (m_actors[i]&&m_actors[i]->m_destroyed) {
			delete m_actors[i];
			m_actors[i]=nullptr;
		}
	}
}
Actor* Map::GetClosestVisibleEnemyWithinRange(ActorHandle* actor) {
	Actor* theActor = GetActorByHandle(*actor);
	float closest = FLT_MAX;
	Actor* result =nullptr;
	for (Actor* enemy : m_actors) {
		if (enemy) {
			if (theActor->CanAggroOnto(enemy->m_handle)) {
				float dist = GetDistanceSquared3D(theActor->m_bottomPosition, enemy->m_bottomPosition);
				if (dist < closest) {
					result = enemy;
					closest = dist;
				}
			}
		}
	}
	return result;
}
void Map::DamageActosInCone(Vec3 pos, EulerAngles orientation, float radius, float cone, FloatRange damage, float impulse, ActorHandle* owner) {
	for (int i = 0; i < m_actors.size(); i++) {
		Actor* actor = m_actors[i];
		//melee only hit marine
		if (actor != nullptr && actor->m_handle->IsValid() && !actor->m_dead&&actor->m_definition->m_base->m_faction==Faction_Actor::MARINE) {
			Vec3 actorPos = actor->m_bottomPosition;
			Vec3 fwd;
			Vec3 j;
			Vec3 k;
			orientation.GetAsVectors_IFwd_JLeft_KUp(fwd, j, k);
			if (IsPointInsideDirectedSector2D(Vec2(actorPos.x, actorPos.y), Vec2(pos.x, pos.y), Vec2(fwd.x, fwd.y), cone, radius)) {
				if (actor->m_definition->m_base->m_faction != GetActorByHandle(*owner)->m_definition->m_base->m_faction) {
					actor->AddImpulse(fwd * impulse);
					actor->Damage(damage);
					SpawnNewActor("DemonScratch", actor->m_bottomPosition);
					//SpawnHitEffect("BloodSplatter", actor->m_bottomPosition + Vec3(0.f,0.f,actor->m_definition->m_camera->m_eyeHeight));
					if (actor->m_AIController && actor->m_controller == actor->m_AIController) {
						actor->m_AIController->DamagedBy(owner);
					}
					if (actor->m_MarineAI && actor->m_controller == actor->m_MarineAI) {
						actor->m_MarineAI->DamagedBy(owner);
					}
				}
			}
		}
	}
}
void Map::RaycastDamage(const Vec3& start, const Vec3& fwd, float dist, FloatRange damage, float impulse, ActorHandle* owner) {
	Actor* projectileOwner = GetActorByHandle(*owner);

	RaycastResult3D worldResult = RaycastAll(start, fwd, dist, projectileOwner);
	/*DebugAddWorldCylinder(start, fwd, .03f, 1.f, Rgba8::WHITE, Rgba8::WHITE, DebugRenderMode::X_RAY);
	if (worldResult.m_didImpact) {
		DebugAddWorldSphere(worldResult.m_impactPos, .06f, 1.f);
		DebugAddWorldArrow(worldResult.m_impactPos, worldResult.m_impactPos + worldResult.m_impactNormal * 0.3f, .03f, 1.f, Rgba8::BLUE, Rgba8::BLUE);
	}*/

	RaycastResult3D finalActorResult;
	Actor* resultActor = nullptr;
	finalActorResult.m_didImpact = false;
	finalActorResult.m_impactDist = FLT_MAX;
	for (int i = 0; i < m_actors.size(); i++) {
		Actor* actor = m_actors[i];
		if (actor) {
			if (actor != projectileOwner && actor->m_definition->m_collision->m_collideWithActor) {
				FloatRange range(actor->m_bottomPosition.z, actor->m_bottomPosition.z + actor->m_definition->m_collision->m_height);
				RaycastResult3D actorResult = RaycastVsZCylinder3D(start, fwd, dist, actor->m_bottomPosition, actor->m_definition->m_collision->m_radius, range);
				if (actorResult.m_didImpact && actorResult.m_impactDist < finalActorResult.m_impactDist) {
					finalActorResult = actorResult;
					resultActor = actor;
				}
			}
		}
	}
	if (finalActorResult.m_didImpact&& finalActorResult.m_impactPos==worldResult.m_impactPos) {
		if (true/*resultActor->m_definition->m_base->m_faction != projectileOwner->m_definition->m_base->m_faction*/) {
			resultActor->AddImpulse(fwd * impulse);

			int damageNum = g_RNG->RollRandomIntInRange((int)damage.m_min, (int)damage.m_max);
			resultActor->Damage(damageNum);
			SpawnNewActor("BloodSplatter", worldResult.m_impactPos);
			if (resultActor->m_AIController && resultActor->m_controller == resultActor->m_AIController) {
				resultActor->m_AIController->DamagedBy(owner);
			}
			if (resultActor->m_MarineAI && resultActor->m_controller == resultActor->m_MarineAI) {
				resultActor->m_MarineAI->DamagedBy(owner);
			}
		}
	}
	else {
		if (worldResult.m_didImpact) {
			SpawnNewActor("BulletHit", worldResult.m_impactPos);
		}
	}
}
void Map::SpawnPlayer() {
	if ((int)m_actors.size() < ActorHandle::MAX_ACTOR_INDEX && m_nextActorUID < ActorHandle::MAX_ACTOR_UID) {
		if (!m_player1Handle || !GetActorByHandle(*m_player1Handle))
		{
			ActorDefinition* def = nullptr;
			m_player1Handle = CreateValidHandle();
			if (!m_gold) {
				def = ActorDefinition::GetActorDefinitionByName("Marine");
			}
			else {
				def = ActorDefinition::GetActorDefinitionByName("Demon");
				DemonAIController::g_party.push_back(m_player1Handle);
			}
			
			Actor* actor = new Actor(def, m_player1Handle);
			actor->m_bottomPosition = Vec3(25.5f, 15.5f, 0.f);
			actor->m_orientation = EulerAngles(270.f, 0.f, 0.f);
			actor->m_velocity = Vec3(0.f, 0.f, 0.f);

			actor->m_map = this;
			if (m_player1Handle->GetIndex() == (int)m_actors.size())
				m_actors.push_back(actor);
			else {
				m_actors[m_player1Handle->GetIndex()] = actor;
			}
			
		}
		
		//--------------------------------------------------------
		if (m_splitScreen) {
			if (!m_player2Handle || !GetActorByHandle(*m_player2Handle)) {
				m_player2Handle = CreateValidHandle();
				ActorDefinition* def = ActorDefinition::GetActorDefinitionByName("Marine");
				Actor* actor = new Actor(def, m_player2Handle);
				actor->m_bottomPosition = Vec3(28.5f, 15.5f, 0.f);
				actor->m_orientation = EulerAngles(270.f, 0.f, 0.f);
				actor->m_velocity = Vec3(0.f, 0.f, 0.f);

				actor->m_map = this;
				if (m_player2Handle->GetIndex() == (int)m_actors.size())
					m_actors.push_back(actor);
				else {
					m_actors[m_player2Handle->GetIndex()] = actor;
				}
			}
		}
	}
}
void Map::DebugPossessNext() {
	/*ActorHandle* target = NextPossessible();
	if(target)
	    g_player1Controller->PlayerPossess(target);*/
}
ActorHandle* Map::NextPossessible() {
	ActorHandle* result = nullptr;
	for (int i = m_currentPossesedID + 1; i < m_actors.size(); i++) {
		if (m_actors[i]) {
			if ((!m_actors[i]->m_dead) && (m_actors[i]->m_definition->m_base->m_canBePossessed)) {
				result = m_actors[i]->m_handle;
				return result;
			}
		}
	}
	//loop back
	for (int i = 0; i < m_currentPossesedID; i++) {
		if (m_actors[i]) {
			if ((!m_actors[i]->m_dead) && (m_actors[i]->m_definition->m_base->m_canBePossessed)) {
				result = m_actors[i]->m_handle;
				return result;
			}
		}
	}
	return nullptr;
}
void Map::AddProjectileToList(Actor* owner, WeaponDefinition* weaponDef, const EulerAngles& deg) {

	if ((int)m_actors.size() < ActorHandle::MAX_ACTOR_INDEX&&m_nextActorUID<ActorHandle::MAX_ACTOR_UID) {
		ActorHandle* handle = CreateValidHandle();
		ActorDefinition* def = ActorDefinition::GetActorDefinitionByName(weaponDef->m_projectileActor);
		Actor* projectile = new Actor(def, handle);
		projectile->m_orientation = deg;
		Vec3 dir = owner->m_orientation.GetAsMatrix_IFwd_JLeft_KUp().TransformVectorQuantity3D(Vec3(owner->m_definition->m_collision->m_radius + .3f, 0.f, owner->m_definition->m_camera->m_eyeHeight));
		projectile->m_bottomPosition = owner->m_bottomPosition + dir;
		
		projectile->m_velocity = projectile->m_orientation.GetAsMatrix_IFwd_JLeft_KUp().TransformVectorQuantity3D(Vec3(weaponDef->m_projectileSpeed, 0.f, 0.f));
		projectile->m_owner = owner->m_handle;
		projectile->m_map = this;
		if(handle->GetIndex()==(int)m_actors.size())
		    m_actors.push_back(projectile);
		else {
			m_actors[handle->GetIndex()] = projectile;
		}
	}
	
}
ActorHandle* Map::SpawnNewActor(std::string name, Vec3 position, Vec3 velocity) {
	if ((int)m_actors.size() < ActorHandle::MAX_ACTOR_INDEX && m_nextActorUID < ActorHandle::MAX_ACTOR_UID) {
		ActorHandle* handle = CreateValidHandle();
		ActorDefinition* def = ActorDefinition::GetActorDefinitionByName(name);
		Actor* hitEffect = new Actor(def, handle);
		hitEffect->m_bottomPosition = position;
		hitEffect->m_velocity = velocity;
		hitEffect->m_map = this;
		if (handle->GetIndex() == (int)m_actors.size())
			m_actors.push_back(hitEffect);
		else {
			m_actors[handle->GetIndex()] = hitEffect;
		}
		return handle;
	}
	return nullptr;
}
ActorHandle* Map::CreateValidHandle() {
	for (int i = 0; i < (int)m_actors.size(); i++) {
		if (m_actors[i] == nullptr) {
			ActorHandle* handle = new ActorHandle(m_nextActorUID, i);
			m_nextActorUID++;
			return handle;
		}
	}
	ActorHandle* handle = new ActorHandle(m_nextActorUID, (int)m_actors.size());
	m_nextActorUID++;
	return handle;
}
PlayerController* Map::GetPlayer(const ActorHandle& handle) {
	if (GetActorByHandle(handle)) {
		if (handle == *m_player1Handle) {
			return g_player1Controller;
		}
		if (handle == *m_player2Handle) {
			return g_player2Controller;
		}
	}
	return nullptr;
}