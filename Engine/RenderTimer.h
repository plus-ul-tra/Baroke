#pragma once
#include "Timer.h"
#include "Singleton.h"



class RenderTimer : public Singleton<RenderTimer>
{
	friend class Singleton<RenderTimer>;
private:
	RenderTimer();
	~RenderTimer() = default;
	Timer& m_engineTimer; 
	float m_deltaTime = 0.0; 
	float m_elapsedTime = 0.0; 

public:
	void Update() {
		m_deltaTime = static_cast<float>(m_engineTimer.GetRealElapsedTime());
		//m_elapsedTime += m_deltaTime;
	}
	float GetDeltaTime() const { return m_deltaTime *	0.2f; }
	float GetDeltaTimeX2() const { return m_deltaTime * 0.5f; }
	//float GetElapsedTime() const { return m_elapsedTime; }

};
