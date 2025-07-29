#pragma once
#include "pch.h"
#include "Core.h"

MIYABI::Core::Core() : m_engineTimer(), m_gameTimer(m_engineTimer) {

}

bool MIYABI::Core::_CreateWindow()
{
	const wchar_t* className = L"Test Game";
	const wchar_t* WindowName = L"Test Game";

	if (FALSE == m_window.Create(className, WindowName, 1920, 1080)) {
		return false;
	}

	return TRUE;

}

void MIYABI::Core::BootManagers()
{
	SceneManager::GetInstance().Initialize(static_cast<HWND>(GetWindowHandle()));
	InputManager::GetInstance().Initialize(static_cast<HWND>(GetWindowHandle()));


	SoundManager::GetInstance().Initialize(); // 사운드 매니저 초기화
	//SoundManager::GetInstance().PlaySoundOnce("TestSound.ogg");

	JokerManager::GetInstance().Initialize(); // 조커 매니저 초기화


}


// https://learn.microsoft.com/ko-kr/windows/win32/inputdev/virtual-key-codes
// F10이 안먹는데 확인좀 - 준혁
bool MIYABI::Core::KeyCommandMapping()
{
	m_keyCommandMap[VK_F1] = "F1";
	m_keyCommandMap[VK_F2] = "F2";
	m_keyCommandMap[VK_F3] = "F3";
	m_keyCommandMap[VK_F4] = "F4";
	m_keyCommandMap[VK_F5] = "F5";
	m_keyCommandMap[VK_F8] = "DebugMode";
	m_keyCommandMap[VK_F12] = "Skip";
	m_keyCommandMap['W'] = "Go";
	m_keyCommandMap['S'] = "Back";
	m_keyCommandMap['A'] = "Left";
	m_keyCommandMap['D'] = "Right";
	m_keyCommandMap[VK_ESCAPE] = "Escape"; // 게임 일시 정지/재개

	return true;
}

void MIYABI::Core::ProcessInput()
{
	InputManager& input = InputManager::GetInstance();

	ProcessKeyInput(input);
	ProcessMouseWheel(input);
	ProcessMouseInput(input);

}

void MIYABI::Core::ProcessKeyInput(InputManager& input)
{

	static const std::unordered_set<int> holdKeys = { 'W', 'A', 'S', 'D', VK_F2 };

	for (int vk = 0; vk < 256; ++vk)
	{
		if (input.GetKeyPressed(vk) || (holdKeys.count(vk) && input.GetKeyDown(vk)))
		{
			auto it = m_keyCommandMap.find(vk);
			if (it != m_keyCommandMap.end())
				SendMessage(it->second);
		}
		else if (vk == 'W' && input.GetKeyDown(vk))
		{
			auto it = m_keyCommandMap.find(vk);
			if (it != m_keyCommandMap.end())
			{
				SendMessage(it->second);
			}
		}
		else if (vk == 'A' && input.GetKeyDown(vk))
		{
			auto it = m_keyCommandMap.find(vk);
			if (it != m_keyCommandMap.end())
			{
				SendMessage(it->second);
			}
		}
		else if (vk == 'S' && input.GetKeyDown(vk))
		{
			auto it = m_keyCommandMap.find(vk);
			if (it != m_keyCommandMap.end())
			{
				SendMessage(it->second);
			}
		}
		else if (vk == 'D' && input.GetKeyDown(vk))
		{
			auto it = m_keyCommandMap.find(vk);
			if (it != m_keyCommandMap.end())
			{
				SendMessage(it->second);
			}
		}
	}
}

void MIYABI::Core::ProcessMouseInput(InputManager& input)
{

	static MouseState prevMouseStatus = InputManager::GetInstance().GetMouseState();

	MouseState curMouseStatus = InputManager::GetInstance().GetMouseState();

	auto push = [&](MouseEvent ev) { SceneManager::GetInstance().PushInput(ev); };

	// 마우스 좌클릭
	if (prevMouseStatus.leftPressed && !curMouseStatus.leftPressed)		// OnLButtonUp
	{
		push({ MouseType::LUp, curMouseStatus.pos ,m_dragStart ,m_dragEnd });
		m_dragEnd = { curMouseStatus.pos.x, curMouseStatus.pos.y };

	}
	else if (!prevMouseStatus.leftPressed && curMouseStatus.leftPressed)	// OnLButtonDown
	{
		push({ MouseType::LDown, curMouseStatus.pos ,m_dragStart ,m_dragEnd });
		m_dragStart = { curMouseStatus.pos.x, curMouseStatus.pos.y };

		//std::cout << curMouseStatus.pos.x << " " << curMouseStatus.pos.y << std::endl;


	}



	// 마우스 우클릭
	if (prevMouseStatus.rightPressed && !curMouseStatus.rightPressed)		// OnRButtonUp
	{

		push({ MouseType::RUp, curMouseStatus.pos ,m_dragStart ,m_dragEnd });

	}
	else if (!prevMouseStatus.rightPressed && curMouseStatus.rightPressed)	// OnRButtonDown
	{
		push({ MouseType::RDown, curMouseStatus.pos ,m_dragStart ,m_dragEnd });
		m_dragStart = { curMouseStatus.pos.x, curMouseStatus.pos.y };

	}

	// 마우스 이동 감지
	if (IsMouseMove(prevMouseStatus, curMouseStatus))
	{
		//if (!curMouseStatus.leftPressed) return;

		m_dragEnd = { curMouseStatus.pos.x, curMouseStatus.pos.y };
		push({ MouseType::Move, curMouseStatus.pos ,m_dragStart ,m_dragEnd });
	}

	prevMouseStatus = curMouseStatus;
}

void MIYABI::Core::ProcessMouseWheel(InputManager& input)
{

	int wheel = InputManager::GetInstance().GetWheelDelta();

	if (wheel > 0) // 추후 줌인 줌아웃 기능 
	{
		std::cout << wheel;
	}
	else if (wheel < 0)
	{
		// 휠 아래로 돌림
		std::cout << wheel;
	}
}

void MIYABI::Core::SendMessage(std::string& _stirng)
{
	SceneManager::GetInstance().OnCommand(_stirng);
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

	KeyCommandMapping();

	
}


void MIYABI::Core::StartGameLoop()
{
	MSG msg = { 0 };
	// message;
	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			if (false == InputManager::GetInstance().OnHandleMessage(msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			ProcessInput();

			// loop
			if (!m_isPause) {
				MainGame();
			}
			InputManager::GetInstance().FrameEnd();
		}
	} 

}


void MIYABI::Core::MainGame()
{
	m_engineTimer.Tick();
	m_gameTimer.Tick();
	double realDeltaTime = m_engineTimer.GetRealDeltaTime();
	//std::cout << "Current realDeltaTime: " << realDeltaTime << std::endl;
	


	if (m_isPause ||m_gameTimer.IsPaused()) {


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
