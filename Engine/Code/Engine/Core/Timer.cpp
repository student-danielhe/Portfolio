#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
Timer::Timer(float period, const Clock* clock)
	:m_period(period)
	, m_clock(clock)
{
	if (clock == nullptr) {
		m_clock = &Clock::GetSystemClock();
	}
}
void Timer::Start() {
	m_startTime = m_clock->GetTotalSeconds();
}

void Timer::Stop() {
	m_startTime = 0.0;
}

double Timer::GetElapsedTime()const{
	if (m_startTime == 0) {
		return 0.0;
	}
	else {
		return m_clock->GetTotalSeconds() - m_startTime;
	}
}
double Timer::GetElapsedFraction()const {
	return GetElapsedTime() / m_period;
}

bool Timer::IsStopped() const{
	return m_startTime == 0;
}

bool Timer::HasPeriodElapsed()const {
	return GetElapsedTime() >= m_period;
}

bool Timer::DecrementPeriodIfElapsed() {
	if (HasPeriodElapsed()) {
		m_startTime += m_period;
		return true;
	}
	else {
		return false;
	}

}