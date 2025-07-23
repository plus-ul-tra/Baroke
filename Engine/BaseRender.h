#pragma once

#include "Component.h"
#include "CoreTypes.h"
//#include "Renderer.h"
class Object;
class Renderer;
class Transform;

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

	virtual void Render(Renderer& renderer) = 0;

	void Update(double deltaTime)override {}
	void OnEvent(const std::string& ev) override {}

	// transform ������Ʈ���� ���� ĳ��
	void SetOwner(Object* owner) override;

	void SetActive(bool isActive) { m_isActive = isActive; }
	bool IsActive() const { return m_isActive; }
	
	// ���� �������� �� �տ�
	void SetOrder(uint32_t z) { m_order = z; }
	uint32_t GetOrder() const { return m_order; }

};