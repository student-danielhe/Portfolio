#include "Engine/Renderer/SpriteAnimDefinition.hpp"
SpriteAnimDefinition::SpriteAnimDefinition(SpriteSheet const& sheet, int startSpriteIndex, int endSpriteIndex,
	float framesPerSecond, SpriteAnimPlaybackType playbackType):
	m_spriteSheet(sheet)
	,m_startSpriteIndex(startSpriteIndex)
	,m_endSpriteIndex(endSpriteIndex)
	,m_framesPerSecond(framesPerSecond)
	,m_playbackType(playbackType)
{

}

SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const {
	if (m_startSpriteIndex == m_endSpriteIndex) {
		return m_spriteSheet.GetSpriteDef(m_startSpriteIndex);
	}
	int spriteIndex = m_startSpriteIndex;
	int frameNum = static_cast<int>(seconds * m_framesPerSecond);
	int animLength = m_endSpriteIndex - m_startSpriteIndex;
	if (m_playbackType == SpriteAnimPlaybackType::ONCE) {
		if (frameNum>animLength) {
			spriteIndex = m_endSpriteIndex;
		}
		else {
			spriteIndex = m_startSpriteIndex + frameNum;
		}
	}
	else if(m_playbackType == SpriteAnimPlaybackType::LOOP){
	    spriteIndex= m_startSpriteIndex + frameNum % animLength;
	}
	else if(m_playbackType == SpriteAnimPlaybackType::PINGPONG){
		int fromStart = frameNum % (animLength * 2 - 2);
		if (fromStart >= animLength) {
			fromStart = animLength - (fromStart-(animLength-1));
		}
		spriteIndex = m_startSpriteIndex + fromStart;
	}
	return m_spriteSheet.GetSpriteDef(spriteIndex);
}
void SpriteAnimDefinition::UpdateStartIndex(int x) {
	int diff = x - m_startSpriteIndex;
	m_startSpriteIndex += diff;
	m_endSpriteIndex += diff;
}