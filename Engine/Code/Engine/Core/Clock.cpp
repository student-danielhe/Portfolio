#include "Engine/Core/Clock.hpp"
#include "Time.hpp"
#include "Engine/Math/MathUtils.hpp"
Clock* g_systemClock = nullptr;
Clock::Clock() {
	if (g_systemClock == nullptr) {
		g_systemClock = this;
	}
	else {
		m_parent = g_systemClock;
		g_systemClock->AddChild(this);
	}
	
}

Clock::Clock(Clock& parent) 
:m_parent(&parent){
	m_parent = &parent;
	parent.AddChild(this);
}

Clock::~Clock() {
	m_parent->RemoveChild(this);
	for (Clock* clockPtr : m_children) {
		clockPtr->m_parent=nullptr;
	}
}

void Clock::Reset() {
	m_lastUpdateTimeInSecond = GetCurrentTimeSeconds();
	m_totalSeconds = 0.0;
	m_frameCount = 0;
}

bool Clock::IsPaused()const {
	return m_isPaused;
}
void Clock::Pause() {
	m_isPaused = true;
}
void Clock::Unpause() {
	m_isPaused = false;
}
void Clock::TogglePause() {
	m_isPaused = !m_isPaused;
}
void Clock::StepSingleFrame() {
	Unpause();
	m_stepSingleFrame = true;
}

void  Clock::SetTimeScale(float timeScale) {
	m_timeScale = (double)timeScale;
}
float Clock::GetTimeScale()const {
	return(float)m_timeScale;
}

double Clock::GetDeltaSeconds()const {
	return m_deltaSecond;
}
double Clock::GetTotalSeconds()const {
	return m_totalSeconds;
}
int   Clock::GetFrameCount()const {
	return m_frameCount;
}

Clock& Clock::GetSystemClock() {
	return *g_systemClock;
}
void Clock::TickSystemClock() {
	GetSystemClock().Tick();
}

void Clock::AddChild(Clock* childClock){
	m_children.push_back(childClock);
}
void Clock::RemoveChild(Clock* childClock) {
	for (int i = 0; i < m_children.size(); i++) {
		if (m_children[i] == childClock) {
			m_children.erase(m_children.begin() + i);
			return;
		}
	}
}

void Clock::Tick() {
	m_deltaSecond = GetCurrentTimeSeconds() - m_lastUpdateTimeInSecond;
	m_deltaSecond = GetClamped((float)m_deltaSecond, 0, (float)m_maxDeltaSeconds);
	m_lastUpdateTimeInSecond = GetCurrentTimeSeconds();
	Advance(m_deltaSecond);
}
void Clock::Advance(double deltaTimeSecond) {
	m_frameCount++;
	
	if (m_isPaused) {
		deltaTimeSecond = 0.0;
	}
	m_deltaSecond =deltaTimeSecond* m_timeScale;
	m_totalSeconds += deltaTimeSecond*m_timeScale;
	for (Clock* childClock : m_children) {
		childClock->Advance(deltaTimeSecond);
	}
	if (m_stepSingleFrame) {
		m_stepSingleFrame = false;
		Pause();
	}
}