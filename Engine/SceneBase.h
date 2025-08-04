#pragma once
//#include "Engine.h"
#include <memory>

#include "Object.h"
#include "Actor.h"
#include "UI.h"
#include "StaticObject.h"
#include "Renderer.h"
#include "InputEvents.h"
#include "Button.h"
//#include "Core.h"

// 구체화는 Client에서
class SceneBase {

private:
	double m_timeScale = 1.0;
	
protected:
	std::vector<std::unique_ptr<Object>> m_objectList;
	std::vector<std::unique_ptr<Button>> m_buttonList;
	std::vector<std::unique_ptr<Object>> m_UIList;
	//render 에 대해서 따로 관리 생각
	std::unordered_map<std::string, std::function<void()>> m_commandMap;
	
public:
	SceneBase() = default;
	virtual ~SceneBase() = default;

	//virtual void Start() = 0;
	virtual void Initialize() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void Render(Renderer& renderer);

	// 안씀
	void RenderObject(Object* obj, Renderer& renderer);

	// Scene에 따라서 구현 필수는 아님
	virtual void FixedUpdate(double fixedDeltaTime);
	virtual void LateUpdate(double deltaTime);
	// Scene 역할에 따라서 안쓸 수도 있음 
	int GetObjectCount() { return m_objectList.size(); }


	virtual void OnEnter() = 0;

	virtual void OnLeave() = 0;

	virtual void KeyCommandMapping() = 0;

	virtual void Reset()
	{
		m_objectList.clear(); 
		// objectList 외에 것도 clear
	}

	virtual void OnCommand(std::string& cmd)  // 입력처리
	{
		auto it = m_commandMap.find(cmd);
		if (it != m_commandMap.end())
		{
			it->second(); // 함수 실행
		}
		else
		{
			std::cout << "Unknown Command: " << cmd << std::endl;
		}
	}

	virtual void OnInput(const MouseEvent& ev) = 0;

};