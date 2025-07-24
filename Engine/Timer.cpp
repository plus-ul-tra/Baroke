#include "pch.h"
#include "Timer.h"

Timer::Timer(): m_realDeltaTime(0.0),m_realElapsedTime(0.0),
							m_frameCounts(0), m_FPSTimer(0.0), m_FPS(0.0)
{
	QueryPerformanceFrequency(&m_frequency);
}

void Timer::Initialize()
{

	QueryPerformanceCounter(&m_lastTick);
	m_realDeltaTime = 0.0f;
	m_realElapsedTime = 0.0f;

	m_frameCounts = 0;
	m_FPSTimer = 0.0;
	m_FPS;
}

void Timer::Tick()
{
	QueryPerformanceCounter(&m_currentTick);

	m_realDeltaTime = static_cast<double> (m_currentTick.QuadPart - m_lastTick.QuadPart) / m_frequency.QuadPart;
	const double MAX_ALLOWED_DELTA_TIME = 0.25;
	if (m_realDeltaTime > MAX_ALLOWED_DELTA_TIME) {
		m_realDeltaTime = MAX_ALLOWED_DELTA_TIME;
	}

	m_realElapsedTime += m_realDeltaTime;
	m_lastTick = m_currentTick;

	m_frameCounts++;
	m_FPSTimer += m_realDeltaTime;

	if (m_FPSTimer >= 1.0) {
		m_FPS = static_cast<double>(m_frameCounts) / m_FPSTimer;
		m_frameCounts = 0;
		m_FPSTimer = 0.0;
	}
}