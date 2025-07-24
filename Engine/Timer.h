#pragma once
#include "Window.h"

// timer : �÷��� ������Ʈ, ������, �������

// Ŭ������ ������ ���� �ʰ�, Timer ���� Engine�� Game�� �Լ��� ȣ�⸸ �ٸ��� ����
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
	//������ ǥ�� render���� text ���
	double GetFPS() const { return m_FPS; }

	void Initialize();
	void Tick();
	
};



// Game timer : �÷��� ����(��ٿ�, �����ð�, ���� ���)
// EngineTimer�� deltaTime�� ������� �۵�
// time scale ���� UI�� time scale ��������

