#pragma once
#include "pch.h"
#include "Core.h"

MIYABI::Core::Core() : m_engineTimer(), m_gameTimer(m_engineTimer) {

}

bool MIYABI::Core::_CreateWindow()
{
	const wchar_t* className = L"Test Game";
	const wchar_t* WindowName = L"Test Game";

	if (FALSE == m_window.Create(className, WindowName, 1600, 800)) {
		return false;
	}

	return TRUE;

}


void MIYABI::Core::BootManagers()
{
	SceneManager::GetInstance().Init(static_cast<HWND>(GetWindowHandle()));
	// AudioManager::GetInstance().Init();
	
}


void MIYABI::Core::Initialize()
{
	
	if (!_CreateWindow())
	{
		std::cerr << "Failed to initialize sample code." << std::endl;
		return ;
	}

	BootManagers();
	m_engineTimer.Initialize();
	m_gameTimer.Initialize();
	
}


void MIYABI::Core::StartGameLoop()
{
	MSG msg = { 0 };
	// message;
	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {

			// loop
			if (!m_pause) {
				MainGame();
			}
			
		}
	}
}


void MIYABI::Core::MainGame()
{
	m_engineTimer.Tick();
	m_gameTimer.Tick();
	double realDeltaTime = m_engineTimer.GetRealDeltaTime();
	//std::cout << "Current realDeltaTime: " << realDeltaTime << std::endl;
	

	if (m_pause ||m_gameTimer.IsPaused()) {

	}
	else {
			double gameDeltaTime = m_gameTimer.GetDeltaTime();
			//std::cout << "Current gameDeltaTime: " << m_gameTimer.GetDeltaTime() << std::endl;

			m_fixedUpdateAccumulator += realDeltaTime;

			while (m_fixedUpdateAccumulator >= m_fixedDeltaTime) {
				SceneManager::GetInstance().FixedUpdate(m_fixedDeltaTime);
				m_fixedUpdateAccumulator -= m_fixedDeltaTime;
			}
			//std::cout << "Current gameDeltaTime: " << gameDeltaTime << std::endl;

			SceneManager::GetInstance().Update(gameDeltaTime); //parm deltaTime
			
			SceneManager::GetInstance().LateUpdate(gameDeltaTime); // deltaTime
		
	}

	SceneManager::GetInstance().Render();

	//Scene 전환 로직은 모든 업데이트 및 그리기 마무리 후에
	//SceneManager::GetInstance().

	
}
