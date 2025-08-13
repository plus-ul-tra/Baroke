#pragma once
#include "cpch.h"
#include "Engine.h"
#include "GameManager.h"


void GameManager::Initialize() //게임 종료, 루프? -> core>
{
	// time 관련
	SetGameScene();
}

void GameManager::SetGameScene() {

	// 게임에 쓸 씬  
	// 객체생성 + upcasting까지 
	SceneManager::GetInstance().AddScene("Title", std::make_unique<TitleScene>()); // index 0 
	SceneManager::GetInstance().AddScene("Game1", std::make_unique<GameScene>());  // index 1 
	SceneManager::GetInstance().AddScene("Ending", std::make_unique<EndingScene>());
	SceneManager::GetInstance().AddScene("Tutorial", std::make_unique<TutoScene>());
	SceneManager::GetInstance().InitializeAllScenes();    //일단 테스트
}

void GameManager::GetModeState()
{
	//m_isDeBug = MIYABI::Core::GetInstance().IsDebugMode(); 
}

