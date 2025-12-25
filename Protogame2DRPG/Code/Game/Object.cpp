#include "Object.hpp"
#include "Map.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Interaction.hpp"
Object::Object(ObjectDefinition* def, Map* map, IntVec2 pos, Interaction* interaction)
{
    m_def = def;
    m_map = map;
	m_position = pos;

	if (interaction) {
		m_interaction = interaction;
		m_interaction->m_owner = this;
	}
	
}

void Object::Interact(Object* other)
{
    if(m_interaction)
        m_interaction->OnInteraction(other);
}

void Object::Render()
{
	g_theRenderer->BindTexture(&m_def->m_spriteSheet->GetTexture());

	std::vector<Vertex_PCU> verts;
	Vec2 mins = m_map->GridToScreen(m_position);
	Vec2 maxs = m_map->GridToScreen(m_position+IntVec2(1, 1));
	
	int index = m_def->m_spriteIndex;
	
	if (m_currentDirection == SpriteDirection::WEST) {
		index = m_def->m_leftIndex;
	}

	if (m_currentDirection == SpriteDirection::EAST) {
		index = m_def->m_rightIndex;
	}

	if (m_currentDirection == SpriteDirection::NORTH) {
		index = m_def->m_backIndex;
	}

	AddVertsForRect(verts, AABB2(mins, maxs), Rgba8::WHITE, m_def->m_spriteSheet->GetSpriteUVs(index));
	g_theRenderer->DrawVertexArray(verts);
}

void Object::Update(float deltaSeconds)
{
	if (m_interaction) {
		m_interaction->OnTick(deltaSeconds);
	}

}

void Object::Move(IntVec2 newPosition)
{

	if (m_def->m_layer == LAYER_CHRACTER) {
		int currentPosIndex =(int) (m_position.x + m_position.y * m_map->m_dimensions.x  );
		int targetPosIndex  =(int) (newPosition.x + newPosition.y * m_map->m_dimensions.x);



		if (newPosition.x == m_position.x) {
			if (newPosition.y == m_position.y + 1) {
				m_currentDirection = SpriteDirection::NORTH;
			}
			else if (newPosition.y == m_position.y - 1) {
				m_currentDirection = SpriteDirection::SOUTH;
			}
		}
		else if (newPosition.y == m_position.y) {
			if (newPosition.x == m_position.x + 1) {
				m_currentDirection = SpriteDirection::EAST;
			}
			else if (newPosition.x == m_position.x - 1) {
				m_currentDirection = SpriteDirection::WEST;
			}
		}

		if (!m_map->IsBlocked(newPosition)&&!m_map->m_chObjects[targetPosIndex]) {
			m_map->m_chObjects[targetPosIndex] = this;
			m_map->m_chObjects[currentPosIndex] = nullptr;
			m_position = newPosition;
			if (m_map->m_bgObjects[targetPosIndex]) {
				m_map->m_bgObjects[targetPosIndex]->Interact(this);
			}
		}
    }
}

Object* Object::GetInteractingObject()
{
    IntVec2 direction;
	if (m_currentDirection == SpriteDirection::EAST) {
		direction = IntVec2(1, 0);
	}

	if (m_currentDirection == SpriteDirection::WEST) {
		direction = IntVec2(-1, 0);
	}

	if (m_currentDirection == SpriteDirection::NORTH) {
		direction = IntVec2(0, 1);
	}

	if (m_currentDirection == SpriteDirection::SOUTH) {
		direction = IntVec2(0, -1);
	}

	IntVec2 pos = m_position + direction;

	Object* obj = m_map->m_chObjects[(int)(pos.x+pos.y*m_map->m_dimensions.x)];

	if (obj) {
		return obj;
	}

	obj = m_map->m_bgObjects[(int)(m_position.x+m_position.y*m_map->m_dimensions.x)];

	if (obj) {
		return obj;
	}

	return nullptr;
}


