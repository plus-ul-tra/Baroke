#pragma once
#include "Singleton.h"
#include "Window.h"
#include "Timer.h"
#include "GameTimer.h"
#include "SceneManager.h"


namespace MIYABI {

	class Core : public Singleton<Core>{

		friend class Singleton<Core>;

	private:
		//HWND hwnd;
		bool m_debugMode = true;
		bool m_pause = false;
		
		//Time 관련
		Timer m_engineTimer; // 조작X
		GameTimer m_gameTimer; // 조작 interface 
		
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

	public:
		void Initialize();
		void StartGameLoop();
		void MainGame();

		void SetbugMode() { m_debugMode = true; }
		bool IsDebugMode()const { return m_debugMode; }
		//loop 조작
		void Pause() { m_pause = true; }
		void Resume() { m_pause = false; }
		
		double GetFps() { return m_engineTimer.GetFPS(); }
	};

}

