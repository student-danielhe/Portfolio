#include "Weapon.hpp"
#include "GameCommon.hpp"
#include "Game.hpp"
Weapon::Weapon(WeaponDefinition* def, ActorHandle* owner)
:m_definition(def)
,m_owner(owner){
	m_timer = new Timer(m_definition->m_refireTime,g_gameClock);
	m_timer->Start();
	m_animClock = new Clock(*g_gameClock);
	if (m_definition->m_HUD) {
		g_theRenderer->CreateOrGetTextureFromFile(m_definition->m_HUD->m_TexturePath.data());
		InitializeAnimGroup();
	}
}
void  Weapon::Fire() {
	
	if (m_timer->HasPeriodElapsed()) {
		SoundID sound = g_theAudio->CreateOrGetSound(GetSoundPath("Fire"), true);
		g_theAudio->StartSoundAt(sound, g_theGame->m_currentLoadedMap->GetActorByHandle(*m_owner)->m_bottomPosition);
		m_currentAction = "Attack";
		
		m_animClock->Reset();
		m_timer->Start();
		Actor* owner = g_theGame->m_currentLoadedMap->GetActorByHandle(*m_owner);
		owner->TryChangeAction("Attack");
		if (owner != nullptr) {
			for (int i = 0; i < m_definition->m_meleeCount; i++) {
				g_theGame->m_currentLoadedMap->DamageActosInCone(owner->m_bottomPosition, owner->m_orientation,
					m_definition->m_meleeRange, m_definition->m_meleeArc, m_definition->m_meleeDamage, m_definition->m_meleeImpulse, m_owner);
			}
			for (int i = 0; i < m_definition->m_projectileCount; i++) {
				EulerAngles fwd;
				if (owner->m_controller == g_player1Controller) {
					fwd = g_player1Controller->m_orientation;
				}
				else {
					fwd = owner->m_orientation;
				}
				fwd.m_yawDegrees = fwd.m_yawDegrees+GetRandomDirectionInCone(m_definition->m_projectileCone);
				g_theGame->m_currentLoadedMap->AddProjectileToList(owner,m_definition,fwd);
			}
			for (int i = 0; i < m_definition->m_rayCount; i++) {
				EulerAngles orientation = owner->m_orientation;
				Vec3 Position = owner->m_bottomPosition + orientation.GetAsMatrix_IFwd_JLeft_KUp().TransformVectorQuantity3D(Vec3(owner->m_definition->m_collision->m_radius + .2f, 0.f, owner->m_definition->m_camera->m_eyeHeight));
				orientation.m_yawDegrees += GetRandomDirectionInCone(m_definition->m_projectileCone);
				Vec3 fwd;
				if (owner->m_controller == g_player1Controller) {
					fwd = g_player1Controller->GetFwdVector();
				}
				else {
					fwd = orientation.GetAsMatrix_IFwd_JLeft_KUp().TransformVectorQuantity3D(Vec3(1.f, 0.f, 0.f));
				}
				g_theGame->m_currentLoadedMap->RaycastDamage(Position, fwd, m_definition->m_rayRange, m_definition->m_rayDamage, m_definition->m_rayImpulse, m_owner);
			}
		}
	}
	
	
}
float Weapon::GetRandomDirectionInCone(float cone) {
	return g_RNG->RollRandomFloatInRange(-cone, cone);
}


void Weapon::InitializeAnimGroup() {
	for (Animation_Wepaon* anim : m_definition->m_HUD->m_anims) {
		
		SpriteSheet* sheet = new SpriteSheet(*g_theRenderer->CreateOrGetTextureFromFile(anim->m_spriteSheetPath.data()), 
			IntVec2((int)anim->m_cellCount.x, (int)anim->m_cellCount.y));
		
		SpriteAnimDefinition* sprdef = new SpriteAnimDefinition(*sheet, anim->m_startFrame, anim->m_endFrame, 
			1.f/anim->m_secoundsPerFrame, SpriteAnimPlaybackType::ONCE);
		m_animationMap.insert_or_assign(anim->m_name, sprdef);
	}
}

std::string Weapon::GetSoundPath(std::string name) {
	for (Sound_Weapon* sound : m_definition->m_sounds) {
		if (sound->m_soundName.compare(name) == 0) {
			return sound->m_soundPath;
		}
	}
	return "";
}
Weapon::~Weapon() {
	if (m_timer) {
		delete m_timer;
		m_timer = nullptr;
	}
	if (m_animClock) {
		delete m_animClock;
		m_animClock = nullptr;
	}
}