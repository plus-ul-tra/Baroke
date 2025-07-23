#pragma once
//#include "Engine.h"
#include <memory>

#include "Object.h"
#include "Actor.h"
#include "UI.h"
#include "StaticObject.h"
#include "Renderer.h"
//#include "Core.h"

// ��üȭ�� Client����
class SceneBase {

private:
	double m_timeScale = 1.0;
	
protected:
	std::vector<std::unique_ptr<Object>> m_objectList;
	//render �� ���ؼ� ���� ���� ����
	
public:
	SceneBase() = default;
	virtual ~SceneBase() = default;

	//virtual void Start() = 0;
	virtual void Initialize() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void Render(Renderer& renderer);

	// Scene�� ���� ���� �ʼ��� �ƴ�
	virtual void FixedUpdate(double fixedDeltaTime);
	virtual void LateUpdate(double deltaTime);
	// Scene ���ҿ� ���� �Ⱦ� ���� ���� 
	int GetObjectCount() { return m_objectList.size(); }

	virtual void OnCommand(std::string& cmd) = 0; // �Է�ó��
};