#pragma once
#include "Singleton.h"
#include "SceneBase.h"
#include "Renderer.h" 
#include "SpriteManager.h" // 일단 씬매니저에 넣어서 Resource폴더에서 전부 로드하게
#include "InputEvents.h"

//GameManager -> SceneManager -> Sences -> Objects XX
// Core -> SceneManager( Scene1, Scene2, Scene3, ) 
//      -> GameManager (                         )
//scene manager에서 무엇을 관리할 것인가

using namespace std;

class SceneManager : public Singleton<SceneManager>
{	//singleton

	friend class Singleton<SceneManager>;

private:

	SceneManager();
	~SceneManager() = default;
	
	int m_currentSceneIndex = -1;
	std::queue<MouseEvent> m_inputQueue;
	//renderer

	std::unique_ptr<Renderer> m_renderer;
	
	std::vector<std::unique_ptr<SceneBase>> m_sceneList;  // 객체 컨테이너
	map<string, int> m_sceneNameMap;					  // just name-index 맵핑, 이건 단순 편의를 위한 것

	// Scene으로 부터 그려야할 obejct를 받을 컨테이너 및 가져올 함수 필요.

public:

	void Initialize(HWND hwnd);


	void FixedUpdate(double fixedDeltaTime);
	void Update(double deltaTime);
	void LateUpdate(double deltaTime);
	//Scene Manager에서 renderer 갖기
	void Render();
	
	int GetCurrentScene() const { return m_currentSceneIndex; }
	void AddScene(const string& sceneName, std::unique_ptr<SceneBase> scene);
	
	// string, int 대응
	template<typename T>
	void ChangeScene(const T& index);
	template<typename T>
	void LoadScene(const T& index);

	void ShowDebugInfo();


	void OnCommand(std::string& cmd);			// 입력처리

	void InitializeAllScenes();

	void  PushInput(const MouseEvent& ev) { m_inputQueue.push(ev); }
	void  DispatchInput();
};


template<typename T>
void SceneManager::ChangeScene(const T& index) {

	//template type에 따라서 분기
	if constexpr (std::is_same_v<T, string>) {
		//m_currentSceneIndex = m_sceneNameMap[index]

		if (auto it = m_sceneNameMap.find(index); it != m_sceneNameMap.end()) {
			m_sceneList[m_currentSceneIndex]->OnLeave();
			m_currentSceneIndex = it->second;
			m_sceneList[m_currentSceneIndex]->OnEnter();
		}
		else { std::cout << "Error: invalid Scene Id" << std::endl; return; }

	}

	else if constexpr (std::is_same_v<T, int>) {
		if (index >= 0 && index < static_cast<int>(m_sceneList.size())) {
			m_currentSceneIndex = index;
		}
		else {
			std::cout << "Error: invalid Scene Index" << std::endl;
			return;
		}
	}
	else { std::cout << "Error: 지원하지 않는 식별자 string or index" << std::endl; return; }
}

template<typename T>
void SceneManager::LoadScene(const T& index) {
	if constexpr (std::is_same_v<T, string>) {
		m_sceneList[m_sceneNameMap[index]]->Initialize();
	}
	else if constexpr (std::is_same_v<T, int>) {
		m_sceneList[index]->Initialize();
	}
	else { std::cout << "Error: 지원하지 않는 식별자 string or index." << std::endl; return; }
}