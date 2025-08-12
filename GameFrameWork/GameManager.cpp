#pragma once
#include "cpch.h"
#include "Engine.h"
#include "GameManager.h"


void GameManager::Initialize() //���� ����, ����? -> core>
{
	// time ����
	SetGameScene();
}

void GameManager::SetGameScene() {

	// ���ӿ� �� ��  
	// ��ü���� + upcasting���� 
	SceneManager::GetInstance().AddScene("Title", std::make_unique<TitleScene>()); // index 0 
	SceneManager::GetInstance().AddScene("Game1", std::make_unique<GameScene>());  // index 1 
	SceneManager::GetInstance().AddScene("Ending", std::make_unique<EndingScene>());
	SceneManager::GetInstance().AddScene("Tutorial", std::make_unique<TutoScene>());
	SceneManager::GetInstance().InitializeAllScenes();    //�ϴ� �׽�Ʈ
}

void GameManager::GetModeState()
{
	//m_isDeBug = MIYABI::Core::GetInstance().IsDebugMode(); 
}

