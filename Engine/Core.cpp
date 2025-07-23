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
	SceneManager::GetInstance().Initialize(static_cast<HWND>(GetWindowHandle()));
	InputManager::GetInstance().Initialize(static_cast<HWND>(GetWindowHandle()));

	// AudioManager::GetInstance().Init();
	
}

// https://learn.microsoft.com/ko-kr/windows/win32/inputdev/virtual-key-codes
bool MIYABI::Core::KeyCommandMapping()
{
	m_keyCommandMap[VK_F1] = "F1";
	m_keyCommandMap[VK_F2] = "F2";
	m_keyCommandMap[VK_F3] = "F3";
	m_keyCommandMap[VK_F4] = "F4";
	m_keyCommandMap[VK_F9] = "DebugMode";
	m_keyCommandMap[VK_F10] = "Skip";
	m_keyCommandMap[VK_ESCAPE] = "Escape"; // ���� �Ͻ� ����/�簳

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

	for (int vk = 0; vk < 256; ++vk)
	{
		if (input.GetKeyPressed(vk))
		{


			auto it = m_keyCommandMap.find(vk);
			if (it != m_keyCommandMap.end())
			{
				SendMessage(it->second);

			}
		}
		else if (vk == VK_F2 && input.GetKeyDown(vk))
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
	// ���콺 ��Ŭ��
	if (prevMouseStatus.leftPressed && !curMouseStatus.leftPressed)
	{
		m_dragEnd = { curMouseStatus.pos.x, curMouseStatus.pos.y };
		std::cout << "Left Mouse Button Released!" << std::endl;			// OnLButtonUp
	}
	else if (!prevMouseStatus.leftPressed && curMouseStatus.leftPressed)	// OnLButtonDown
	{
		m_dragStart = { curMouseStatus.pos.x, curMouseStatus.pos.y };

		std::cout << "Left Mouse Button Pressed!" << std::endl;

	}



	// ���콺 ��Ŭ��
	if (prevMouseStatus.rightPressed && !curMouseStatus.rightPressed)		// OnRButtonUp
	{

		std::cout << "Right Mouse Button Released!" << std::endl;
	}
	else if (!prevMouseStatus.rightPressed && curMouseStatus.rightPressed)	// OnRButtonDown
	{

		std::cout << "Right Mouse Button Pressed!" << std::endl;
	}

	// ���콺 �̵� ����
	if (IsMouseMove(prevMouseStatus, curMouseStatus))
	{
		//if (!curMouseStatus.leftPressed) return;

		m_dragEnd = { curMouseStatus.pos.x, curMouseStatus.pos.y };
		/*std::cout << "Mouse Moved: "
			<< "X: " << prevMouseStatus.pos.x
			<< ", Y: " << prevMouseStatus.pos.y
			<< "=>  X: " << curMouseStatus.pos.x
			<< ", Y: " << curMouseStatus.pos.y
			<< std::endl;*/
	}

	prevMouseStatus = curMouseStatus;
}

void MIYABI::Core::ProcessMouseWheel(InputManager& input)
{

	int wheel = InputManager::GetInstance().GetWheelDelta();

	if (wheel > 0) // ���� ���� �ܾƿ� ��� 
	{
		std::cout << wheel;
	}
	else if (wheel < 0)
	{
		// �� �Ʒ��� ����
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

	//Scene ��ȯ ������ ��� ������Ʈ �� �׸��� ������ �Ŀ�
	//SceneManager::GetInstance().

	
}
