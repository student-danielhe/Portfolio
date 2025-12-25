#pragma once
#include "Engine/Audio/AudioSystem.hpp"
struct GameConfig {
	float m_musicVolume = .1f;
	SoundID m_MenuMusic = 0;
	SoundPlaybackID m_MenuPlayBack = 0;
	SoundID m_gameMusic = 0;
	SoundPlaybackID m_GamePlayBack = 0;
	SoundID m_buttonClickSound = 0;
};