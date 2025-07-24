#pragma once
#include "Timer.h"
class GameTimer
{
private:
	double m_timeScale;
	bool m_isPaused;
	Timer& m_engineTimer;
	
public:
	GameTimer(Timer& engineTimer);
	~GameTimer() = default;
	void Initialize();
	void Tick();

	double GetDeltaTime() const;
	double GetElapsedTime() const;

	double GetTimeScale() const { return m_timeScale; }
	void SetTimeScale(double scale) { m_timeScale = scale; if (scale < 0.0) m_timeScale = 0.0; }

	void Pause()  { m_isPaused = true; }
	void Resume() { m_isPaused = false; }
	bool IsPaused() const { return m_isPaused; }

};