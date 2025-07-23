#include "pch.h"
#include "SceneManager.h"
#include "Core.h"

SceneManager::SceneManager()
{
	std::cout << "SceneManager success"<<std::endl;
}

void SceneManager::Init(HWND hwnd)
{
	
	m_renderer = make_unique<Renderer>();
	m_renderer->Initialize(hwnd);
	m_currentSceneIndex = 0;
}


// update -> fixedupdate -> lateupdate

void SceneManager::Update(double deltaTime)
{
	//only currentScene
	m_sceneList[m_currentSceneIndex]->Update(deltaTime);
}

void SceneManager::FixedUpdate(double fixedDeltaTime) {
	m_sceneList[m_currentSceneIndex]->FixedUpdate(fixedDeltaTime);
}


void SceneManager::LateUpdate(double deltaTime)
{
	m_sceneList[m_currentSceneIndex]->LateUpdate(deltaTime);
}

void SceneManager::Render()
{
	m_renderer->RenderBegin();
	m_sceneList[m_currentSceneIndex]->Render(*m_renderer);

	//�������� ����� ��� �������
	if (MIYABI::Core::GetInstance().IsDebugMode()) {
		ShowDebugInfo();
	}
	
	m_renderer->RenderEnd();
}

void SceneManager::AddScene(const std::string& sceneName, std::unique_ptr<SceneBase> scene)
{
	std::cout << sceneName << " added" << std::endl;
	if (m_sceneNameMap.count(sceneName) > 0) {
		//�ߺ� üũ
		return;
	}
	//����
	// 0 start
	m_sceneList.emplace_back(std::move(scene));                     
	m_sceneNameMap[sceneName] = m_sceneList.size() - 1;

}

void SceneManager::ShowDebugInfo()
{
	float yOffset = 10.0f; // ���� y ��ġ
	float lineHeight = 25.0f; 
	

	// FPS ǥ��
	std::wstringstream ss;
	ss << std::fixed ;
	ss.str(L"");
	ss << L"FPS: " << MIYABI::Core::GetInstance().GetFps();
	std::wstring fps_str = ss.str();
	//m_renderer->DrawMessage(fps_str.c_str(), 10.0f, yOffset, 200.0f, lineHeight, D2D1::ColorF::Red); ���� �����߻�
	yOffset += lineHeight;

}


