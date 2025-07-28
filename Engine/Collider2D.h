#pragma once
#include "Component.h"
#include "Transform.h"
#include <DirectXMath.h>
//#include <DirectXCollision.h>

//Collider의 주 역할은 충돌 check
// 구현 Enter, exit, stay

// position에 대한 것은 Transform position 에 종속
using namespace DirectX;

enum class ColliderType
{
	Rectangle,
	Circle
};

class Collider2D : public Component
{
	// 랜더링 관련
private:
	// Transform 정보 필요
	Transform* m_transform = nullptr;

	ColliderType m_type;

	float m_posX;//transform 참조
	float m_posY;//transform 참조
	
	float m_width;
	float m_height;
	float m_radius;

	XMFLOAT4 m_color;

	void GetTransform();
	
public:
	Collider2D();
	~Collider2D() = default;

	void SetColliderType(ColliderType type) { m_type = type; }
	void SetColor(XMFLOAT4 color) { m_color = color; }
	void SetSize(float width, float height);
	void SetOwner(Object* owner) override;


	float GetWidth()               const { return m_width;  }
	float GetHeight()              const { return m_height; }
	float GetRadius()              const { return m_radius; }
	ColliderType GetColliderType() const { return m_type;   }

	D2D1::ColorF GetColor() const {
		return {m_color.x, m_color.y, m_color.z, m_color.w};
	}
	
private:
	//collider On/Off 용
	bool m_isActive = true;
public:
	void FixedUpdate(double deltaTime) override; //<- 구현 필수
	void Update(double deltaTime) override {};
	void OnEvent(const std::string& ev) override {};

	bool IsActive() const { return m_isActive; }
	bool ColliderEnter();
	
};