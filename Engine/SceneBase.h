#pragma once
//#include "Engine.h"
#include <memory>

#include "Object.h"
#include "Actor.h"
#include "UI.h"
#include "StaticObject.h"
#include "Renderer.h"
//#include "Core.h"

// 구체화는 Client에서
class SceneBase {

private:
	double m_timeScale = 1.0;
	
protected:
	std::vector<std::unique_ptr<Object>> m_objectList;
	//render 에 대해서 따로 관리 생각
	
public:
	SceneBase() = default;
	virtual ~SceneBase() = default;

	//virtual void Start() = 0;
	virtual void Initialize() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void Render(Renderer& renderer);

	// Scene에 따라서 구현 필수는 아님
	virtual void FixedUpdate(double fixedDeltaTime);
	virtual void LateUpdate(double deltaTime);
	// Scene 역할에 따라서 안쓸 수도 있음 
	int GetObjectCount() { return m_objectList.size(); }

	virtual void OnCommand(std::string& cmd) = 0; // 입력처리
};