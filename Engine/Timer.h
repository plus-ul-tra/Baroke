#pragma once
#include "Window.h"

// timer : 플레임 업데이트, 렌더링, 물리계산

// 클래스를 별도로 두지 않고, Timer 에서 Engine용 Game용 함수로 호출만 다르게 하자
class Timer {
private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_lastTick;
	LARGE_INTEGER m_currentTick;

	double m_realDeltaTime;
	double m_realElapsedTime;

	long long m_frameCounts;
	double m_FPSTimer;
	double m_FPS;

public:
	Timer();
	~Timer() = default;

	// Engine
	double GetRealDeltaTime() const { return m_realDeltaTime; }
	double GetRealElapsedTime() const { return m_realElapsedTime; }
	//프레임 표시 render에서 text 출력
	double GetFPS() const { return m_FPS; }

	void Initialize();
	void Tick();
	
};



// Game timer : 플레이 로직(쿨다운, 스폰시간, 게임 배속)
// EngineTimer의 deltaTime을 기반으로 작동
// time scale 조작 UI등 time scale 선택적용

