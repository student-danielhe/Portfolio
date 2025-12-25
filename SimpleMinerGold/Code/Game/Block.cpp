#include "Block.hpp"
int Block::GetIndoor()
{
    return (int)m_lightData&LIGHT_MASK_INDOOR;
}

void Block::SetIndoor(int data)
{
   m_lightData =(uint8_t) ((m_lightData&LIGHT_MASK_OUTDOOR)|data);
}

int Block::GetOutdoor()
{
    return (int)(m_lightData&LIGHT_MASK_OUTDOOR)>>LIGHT_BITS_INDOOR;
}
void Block::SetOutdoor(int data)
{
	m_lightData = (uint8_t)((m_lightData & LIGHT_MASK_INDOOR) | (data<<LIGHT_BITS_INDOOR));
}

bool Block::GetIsSky()
{
    return (m_lightFlag&BLOCK_BIT_MASK_IS_SKY) == BLOCK_BIT_MASK_IS_SKY;
}

void Block::SetIsSky(bool input)
{
    if (input) {
        m_lightFlag = m_lightFlag  | BLOCK_BIT_MASK_IS_SKY;
    }
    else {
        m_lightFlag = (m_lightFlag &(!BLOCK_BIT_MASK_IS_SKY));
    }
}

bool Block::GetIsLightDirty()
{
	return (m_lightFlag & BLOCK_BIT_MASK_IS_LIGHT_DIRTY) == BLOCK_BIT_MASK_IS_LIGHT_DIRTY;
}

void Block::SetIsLightDirty(bool input)
{
	if (input) {
		m_lightFlag = m_lightFlag | BLOCK_BIT_MASK_IS_LIGHT_DIRTY;
	}
	else {
		m_lightFlag = (m_lightFlag & (!BLOCK_BIT_MASK_IS_LIGHT_DIRTY));
	}
}

bool Block::GetIsFullOpaque()
{
	return (m_lightFlag & BLOCK_BIT_MASK_IS_FULL_OPAQUE) == BLOCK_BIT_MASK_IS_FULL_OPAQUE;
}


void Block::SetIsFullOpaque(bool input)
{
	if (input) {
		m_lightFlag = m_lightFlag | BLOCK_BIT_MASK_IS_FULL_OPAQUE;
	}
	else {
		m_lightFlag = (m_lightFlag & (!BLOCK_BIT_MASK_IS_FULL_OPAQUE));
	}
}

bool Block::GetIsSolid()
{
	return (m_lightFlag & BLOCK_BIT_MASK_IS_SOLID) == BLOCK_BIT_MASK_IS_SOLID;
}

void Block::SetIsSolid(bool input)
{
	if (input) {
		m_lightFlag = m_lightFlag | BLOCK_BIT_MASK_IS_SOLID;
	}
	else {
		m_lightFlag = (m_lightFlag & (!BLOCK_BIT_MASK_IS_SOLID));
	}
}

bool Block::GetIsVisible()
{
	return (m_lightFlag & BLOCK_BIT_MASK_IS_VISIBLE) == BLOCK_BIT_MASK_IS_VISIBLE;
}

void Block::SetIsVisible(bool input)
{
	if (input) {
		m_lightFlag = m_lightFlag | BLOCK_BIT_MASK_IS_VISIBLE;
	}
	else {
		m_lightFlag = (m_lightFlag & (!BLOCK_BIT_MASK_IS_VISIBLE));
	}
}

bool Block::SetNewTemperature(float input)
{
    BlockDefinition* def = BlockDefinition::g_blockDefinitions[m_type];
	
	if (input < def->m_freezingPoint) {
		m_type = def->m_freezeTo;
		return true;
	}

	if (input > def->m_meltingPoint) {
		m_type = def->m_meltTo;
		return true;
	}

	return false;
}
