#pragma once
#include "Singleton.h"
#include "Window.h"
#include "Timer.h"
#include "GameTimer.h"
#include "SceneManager.h"
#include"InputManager.h"



namespace MIYABI {

	class Core : public Singleton<Core>{

		friend class Singleton<Core>;

	private:
		//HWND hwnd;
		bool m_debugMode = true;

		bool m_isPause = false;

		
		//Time °ü·Ã
		Timer m_engineTimer; // Á¶ÀÛX
		GameTimer m_gameTimer; // Á¶ÀÛ interface 
		
		const double m_fixedDeltaTime = 0.02; // 50Fps
		double m_fixedUpdateAccumulator;

		Window m_window;

		Core(); 
		 ~Core() = default;

		//Window Setting
		bool _CreateWindow();
		void* GetWindowHandle() const { return m_window.GetHandle(); }


		// Manager BootStrap
		void BootManagers();



		//------------------------------------ InputManager Start
		bool KeyCommandMapping();	// Å°¿Í Ä¿¸Çµå ¸ÅÇÎ

		void ProcessInput();		// ÀÔ·Â Ã³¸®
		void ProcessKeyInput(InputManager& input);
		void ProcessMouseInput(InputManager& input);
		void ProcessMouseWheel(InputManager& input);

		void SendMessage(std::string& _stirng);

	private:
		POINT m_dragStart = { 0, 0 };
		POINT m_dragEnd = { 0, 0 };
		std::unordered_map<int, std::string> m_keyCommandMap;


		//------------------------------------ InputManager End



	public:
		void Initialize();
		void StartGameLoop();
		void MainGame();

		void SetbugMode() { m_debugMode = true; }
		bool IsDebugMode()const { return m_debugMode; }
		//loop Á¶ÀÛ

		void Pause() { m_isPause = true; }
		void Resume() { m_isPause = false; }

		
		double GetFps() { return m_engineTimer.GetFPS(); }
	};

}

