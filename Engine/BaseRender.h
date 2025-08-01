#pragma once

#include "Component.h"
#include "CoreTypes.h"
#include <string>
//#include "Renderer.h"
class Object;
class Renderer;
class Transform;
using namespace std;

// �⺻����(����׿�), sprite�� ���� �Ļ�
class BaseRender :public Component{
protected:

	// ������θ� ��üȭ ����
	BaseRender();

	bool m_isActive;
	//���� ����
	uint32_t m_order;
	// Ʈ������ ĳ��
	Transform* m_transform = nullptr;

public :
	virtual ~BaseRender() = default;

	virtual void Render(Renderer& renderer, const string& shadermode) = 0;

	void Update(double deltaTime) override {}
	void OnEvent(const string& ev) override {}

	void SetOwner(Object* owner) override;

	void SetActive(bool isActive) { m_isActive = isActive; }
	bool IsActive() const { return m_isActive; }
	
	// ���� �������� �� �տ�
	void SetOrder(uint32_t z) { m_order = z; }
	uint32_t GetOrder() const { return m_order; }

};