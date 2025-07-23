#pragma once

#include "Component.h"
#include "CoreTypes.h"
//#include "Renderer.h"
class Object;
class Renderer;
class Transform;

// 기본도형(디버그용), sprite용 으로 파생
class BaseRender :public Component{
protected:

	// 상속으로만 객체화 가능
	BaseRender();

	bool m_isActive;
	//랜더 오더
	uint32_t m_order;
	// 트랜스폼 캐시
	Transform* m_transform = nullptr;

public :
	virtual ~BaseRender() = default;

	virtual void Render(Renderer& renderer) = 0;

	void Update(double deltaTime)override {}
	void OnEvent(const std::string& ev) override {}

	// transform 컴포넌트에서 정보 캐싱
	void SetOwner(Object* owner) override;

	void SetActive(bool isActive) { m_isActive = isActive; }
	bool IsActive() const { return m_isActive; }
	
	// 값이 높을수록 더 앞에
	void SetOrder(uint32_t z) { m_order = z; }
	uint32_t GetOrder() const { return m_order; }

};