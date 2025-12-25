#include "PlayerCharacter.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
PlayerCharacter::PlayerCharacter(Map* owner):Entity(owner)
{
	m_ib = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));
	m_vb = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_physicsTimer = new Timer(PHYSICS_FRAMERATE, g_gameClock);
	m_physicsTimer->Start();
	std::vector<Vertex_PCUTBN> verts;
	std::vector<unsigned int> indeces;

	AABB3 localBody(Vec3(-m_playerWidth/2.f, -m_playerWidth/2.f, 0.f), Vec3(m_playerWidth/2.f, m_playerWidth/2.f, m_playerHeight));

	AddVertsForAABB3D(verts, indeces, localBody);

	g_theRenderer->CopyIndexedVertexCPUTBNToBuffers(verts, indeces, m_vb, m_ib);
}

PlayerCharacter::~PlayerCharacter()
{
	delete m_vb;
	m_vb = nullptr;

	delete m_ib;
	m_ib = nullptr;
}

void PlayerCharacter::Update(float deltaSeconds)
{
    m_orientation = m_camera->m_orientation;
	HandleKeyPressed(deltaSeconds);
	if (m_physicsState == PhysicsStates::NOCLIP) {
	    while (m_physicsTimer->DecrementPeriodIfElapsed()) {
		    NoClipUpdate();
		}
	}

	else {
		
		while (m_physicsTimer->DecrementPeriodIfElapsed()) {

			PhysicsUpdate(PHYSICS_FRAMERATE);

		}

		UpdateBody();
		PushOutOfBlocks();
		UpdateBody();
	}
	
    
}



void PlayerCharacter::PhysicsUpdate(float deltaSeconds)
{
    if (m_physicsState != PhysicsStates::NOCLIP && !m_dead) {

        
        ApplyGravityAndDrag();

        m_velocity += m_acceleration;
		m_acceleration = Vec3();

        ClampSpeed();


        m_bottomPosition += m_velocity * deltaSeconds;
		
		UpdateBody();
    }
}



void PlayerCharacter::HandleKeyPressed(float deltaSeconds)
{
	if (g_theInput->WasKeyJustReleased('V')) {
		CyclePhysicsMode();
	}
	if (m_physicsState != PhysicsStates::NOCLIP) {
		float runforce = 0.f;;
		if (m_grounded || m_physicsState == PhysicsStates::FLYING) {
			runforce = m_runForce / 2.f;
		}
		else {
			runforce = m_flyForce;
		}


		Vec3 horizontalNormal = GetForwardNormal();
		horizontalNormal.z = 0.f;
		horizontalNormal = horizontalNormal.GetNormalized();
		if (g_theInput->WasKeyJustPressed('W')) {
			AddForce(horizontalNormal * runforce);
		}

		if (g_theInput->WasKeyJustPressed('D')) {
			AddForce(horizontalNormal.GetRotatedAboutZDegrees(270.f) * runforce);
		}

		if (g_theInput->WasKeyJustPressed('S')) {
			AddForce(-horizontalNormal * runforce);
		}

		if (g_theInput->WasKeyJustPressed('A')) {
			AddForce(horizontalNormal.GetRotatedAboutZDegrees(90.f) * runforce);
		}

		if (m_physicsState == PhysicsStates::FLYING) {
			if (g_theInput->WasKeyJustPressed('Q')) {
			    m_bottomPosition.z+=20.f*deltaSeconds;
			}

			if (g_theInput->WasKeyJustPressed('E')) {
			    m_bottomPosition.z-=20.f*deltaSeconds;
			}
		}

		if (g_theInput->WasKeyJustReleased(' ')) {
			if (m_grounded) {
				AddForce(Vec3(0.f, 0.f, m_jumpImpulse * 3.f));
				m_grounded = false;
			}

		}
	}

}

void PlayerCharacter::Render()const
{
    
	g_theRenderer->BindTexture(nullptr);
	Mat44 modelTransform = Mat44::MakeTranslation3D(m_bottomPosition);
	g_theRenderer->SetModelConstants(modelTransform);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);
	g_theRenderer->DrawIndexedVertexBuffer(m_vb, m_ib);
    
}

void PlayerCharacter::CyclePhysicsMode()
{
    

	if (m_physicsState == PhysicsStates::WALKING) {
		m_physicsState = PhysicsStates::FLYING;
	}

	else if (m_physicsState == PhysicsStates::FLYING) {
		m_physicsState = PhysicsStates::NOCLIP;
	}
	else {
		m_physicsState = PhysicsStates::WALKING;
	}
}

void PlayerCharacter::AddForce(Vec3 force)
{
    m_acceleration += force;
}

void PlayerCharacter::AddImpulse(Vec3 impulse)
{
    m_velocity += impulse;
}

void PlayerCharacter::PushOutOfBlocks()
{
   m_grounded = false;
   std::vector<IntVec3> adjCoord = GetOverlapingBlocks();
   Chunk* chunk = GetCurrentChunk();
   IntVec3 coords = GetCoordInChunk();
   IntVec3 offsets;
   
   if(chunk)
       offsets = IntVec3(chunk->m_coords.x * CHUNK_SIZE_X, chunk->m_coords.y * CHUNK_SIZE_Y, 0);

   for (IntVec3 adj : adjCoord) {
	   Block* b = chunk->GetBlockAtPos(adj);
	   if (b&&b->m_type!=BLOCK_TYPE_AIR&&b->m_type!=BLOCK_TYPE_WATER) {
	       AABB3 globalCollision = AABB3(Vec3((float)adj.x+offsets.x, (float)adj.y+offsets.y, (float)adj.z), Vec3((float)adj.x + 1.f+offsets.x, (float)adj.y+offsets.y + 1.f, (float)adj.z + 1.f));
		   if ((float)adj.z != m_bottomPosition.z) {
			   if ((float)adj.z < m_bottomPosition.z) {
			       PushAABB3OutOfAABB3(m_body, globalCollision, true);
				   m_grounded = true;
			   }
			  /* else {
			       PushAABB3OutOfAABB3(m_body, globalCollision, true);
			   }*/
		           
		   }
		   else {
		       PushAABB3OutOfAABB3(m_body, globalCollision, false);
		   }
	   }
   }

   m_bottomPosition = m_body.m_mins + Vec3(m_body.GetDimensions().x/2.f, m_body.GetDimensions().y/2.f, 0.f);
}

Vec3 PlayerCharacter::GetForwardNormal()
{
	return m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D();
}

bool PlayerCharacter::IsGrounded(float tolerance)
{
    return fabsf(m_bottomPosition.z - GetGroundHeight())<tolerance;
}

void PlayerCharacter::ClampSpeed()
{
	m_horizontalVelocity = Vec2(m_velocity.x, m_velocity.y);
	m_verticalVelocity = m_velocity.z;

	Vec2 runNormal = m_horizontalVelocity.GetNormalized();

	if (m_horizontalVelocity.GetLengthSquared() > m_runSpeed * m_runSpeed) {
		m_horizontalVelocity = runNormal * m_runSpeed;
	}

	if (m_verticalVelocity > m_vertSpeed) {
		m_verticalVelocity = m_vertSpeed;
	}

	if (m_verticalVelocity < -m_vertSpeed) {
		m_verticalVelocity = -m_vertSpeed;
	}

	m_velocity = Vec3(m_horizontalVelocity.x, m_horizontalVelocity.y, m_verticalVelocity);
}

void PlayerCharacter::ApplyGravityAndDrag()
{
	if (m_physicsState != PhysicsStates::FLYING && !IsGrounded()) {
		Vec3 fg = Vec3(0.f, 0.f, -9.8f*PHYSICS_FRAMERATE);
		AddForce(fg);
	}

	m_horizontalVelocity = Vec2(m_velocity.x, m_velocity.y);



	if (IsGrounded()) {
		Vec2 velNormal = m_horizontalVelocity.GetNormalized();
		AddForce(m_drag * -Vec3(velNormal.x, velNormal.y, 0.f)*PHYSICS_FRAMERATE);
		m_verticalVelocity = 0.f;
		m_velocity.z = 0.f;
		//m_grounded = true;

	}
	else {
		Vec3 velNormal = m_velocity.GetNormalized();
		AddForce(m_drag * -velNormal*PHYSICS_FRAMERATE);
	}
}

void PlayerCharacter::UpdateBody()
{
    m_body.m_mins = Vec3(m_bottomPosition.x - m_playerWidth/2.f, m_bottomPosition.y - m_playerWidth/2.f, m_bottomPosition.z);
	m_body.m_maxs = Vec3(m_bottomPosition.x + m_playerWidth/2.f, m_bottomPosition.y + m_playerWidth/2.f, m_bottomPosition.z + m_playerHeight);
}

void PlayerCharacter::SetPosition(Vec3 pos)
{
    m_bottomPosition = pos;
	UpdateBody();
}

void PlayerCharacter::NoClipUpdate()
{
    float speed = 20.f;
	m_velocity = Vec3();
	m_orientation = m_camera->m_orientation;
	if (g_theInput->WasKeyJustPressed('W')) {
		m_velocity.x = speed;
	}
	else if (g_theInput->WasKeyJustPressed('S')) {
		m_velocity.x = -speed;
	}

	if (g_theInput->WasKeyJustPressed('A')) {
		m_velocity.y = speed;
	}
	else if (g_theInput->WasKeyJustPressed('D')) {
		m_velocity.y = -speed;
	}

	m_velocity = m_orientation.GetAsMatrix_IFwd_JLeft_KUp().TransformVectorQuantity3D(m_velocity);
	

	
	m_bottomPosition += m_velocity * PHYSICS_FRAMERATE;
    UpdateBody();
}

Chunk* PlayerCharacter::GetCurrentChunk()
{
	float x = m_body.m_mins.x;
	float y = m_body.m_mins.y;

	int chunkCoordX = 0;
	while (x >= (float)CHUNK_SIZE_X) {
	    x-=CHUNK_SIZE_X;
		chunkCoordX++;
	}

	while (x < 0) {
		x+=CHUNK_SIZE_X;
		chunkCoordX--;
	}

	int chunkCoordY = 0;
	while (y >= (float)CHUNK_SIZE_Y) {
		y -= CHUNK_SIZE_Y;
		chunkCoordY++;
	}

	while (y < 0) {
		y += CHUNK_SIZE_Y;
		chunkCoordY--;
	}

	return m_owner->GetChunkAtPos(IntVec2(chunkCoordX, chunkCoordY));
}

IntVec3 PlayerCharacter::GetCoordInChunk()
{
	float x = m_body.m_mins.x;
	float y = m_body.m_mins.y;

	int chunkCoordX = 0;
	while (x >= (float)CHUNK_SIZE_X) {
		x -= CHUNK_SIZE_X;
		chunkCoordX++;
	}

	while (x < 0) {
		x += CHUNK_SIZE_X;
		chunkCoordX--;
	}

	int chunkCoordY = 0;
	while (y >= (float)CHUNK_SIZE_Y) {
		y -= CHUNK_SIZE_Y;
		chunkCoordY++;
	}

	while (y < 0) {
		y += CHUNK_SIZE_Y;
		chunkCoordY--;
	}

	return IntVec3((int)floorf(x), (int)floorf(y), (int)floorf(m_bottomPosition.z));
}

float PlayerCharacter::GetGroundHeight()
{
    IntVec3 localCoords = GetCoordInChunk();

	std::vector<int> groundHeights;
	
	Chunk* currentChunk = GetCurrentChunk();

	groundHeights.push_back(currentChunk->FindFirstSolidBlockBelow(localCoords));

	if (m_body.m_maxs.x > (float)floor(m_body.m_mins.x) + 1.f) {
	    groundHeights.push_back(currentChunk->FindFirstSolidBlockBelow(IntVec3(localCoords.x+1, localCoords.y, localCoords.z)));
	}

	if (m_body.m_maxs.y > (float)floor(m_body.m_mins.y) + 1.f) {
	    groundHeights.push_back(currentChunk->FindFirstSolidBlockBelow(IntVec3(localCoords.x, localCoords.y+1, localCoords.z)));
	}

	if ((m_body.m_maxs.x > (float)floor(m_body.m_mins.x) + 1.f) && (m_body.m_maxs.y > (float)floor(m_body.m_mins.y) + 1.f)) {
	    groundHeights.push_back(currentChunk->FindFirstSolidBlockBelow(IntVec3(localCoords.x+1, localCoords.y+1, localCoords.z)));
	}

	int highest = 0;

	for (int i : groundHeights) {
		if (i > highest) {
			highest = i;
		}
	}

	return (float)highest;
}

std::vector<IntVec3> PlayerCharacter::GetOverlapingBlocks()
{
    IntVec3 localCoords = GetCoordInChunk();
	std::vector<IntVec3> result;
	AABB3 localBody = GetLocalBody();
	for (int x = localCoords.x; x <= localCoords.x + 1; x++) {
		for (int y = localCoords.y; y <= localCoords.y + 1; y++) {
			for (int z = localCoords.z-1; z <= localCoords.z + 2; z++) {
				AABB3 blockCollision;
				blockCollision.m_mins = Vec3((float)x, (float)y, (float)z);
				blockCollision.m_maxs = Vec3((float)x, (float)y, (float)z) + Vec3(1.f, 1.f, 1.f);

				if (DoAABB3sOverlap(blockCollision, localBody)) {
				    result.push_back(IntVec3(x, y, z));
				}
			}
		}
	}

	return result;
}

AABB3 PlayerCharacter::GetLocalBody()
{
	float x = m_body.m_mins.x;
	float y = m_body.m_mins.y;

	int chunkCoordX = 0;
	while (x >= (float)CHUNK_SIZE_X) {
		x -= CHUNK_SIZE_X;
		chunkCoordX++;
	}

	while (x < 0) {
		x += CHUNK_SIZE_X;
		chunkCoordX--;
	}

	int chunkCoordY = 0;
	while (y >= (float)CHUNK_SIZE_Y) {
		y -= CHUNK_SIZE_Y;
		chunkCoordY++;
	}

	while (y < 0) {
		y += CHUNK_SIZE_Y;
		chunkCoordY--;
	}

	return AABB3(Vec3(x,y,m_bottomPosition.z), Vec3(x+m_playerWidth, y+m_playerWidth, m_bottomPosition.z+m_playerHeight));
}
