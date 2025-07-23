#include "pch.h"
#include "GameTimer.h"

GameTimer::GameTimer(Timer& engineTimer) : m_engineTimer(engineTimer), m_timeScale(1.0), m_isPaused(false)
{

}

void GameTimer::Initialize()
{
	
	m_isPaused = false;
	m_timeScale = 1.0;
}


void GameTimer::Tick()
{

}

double GameTimer::GetDeltaTime() const
{
	if (m_isPaused) {
		return 0.0;
	}
	//타임 스캐일 적용
	return m_engineTimer.GetRealDeltaTime() * m_timeScale;
}

double GameTimer::GetElapsedTime() const
{
	return 0.0;
}