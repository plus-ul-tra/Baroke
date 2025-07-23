#pragma once
#include "Component.h"
#include "CoreTypes.h"
//#include <dcommon.h>
#include <DirectXMath.h>

class GameObject;
using namespace DirectX;

class Transform : public Component {
private:
	XMVECTOR m_position;
	XMVECTOR m_scale;
	float m_rotationRad;

	Transform* m_parent = nullptr;


public:
	Transform();
	Transform(XMVECTOR pos, float rotRad, XMVECTOR scale = { 1.0f, 1.0f });
	virtual ~Transform() = default;

	virtual void Initialize();
	virtual void Update(double deltaTime) override;
	virtual void OnEvent(const std::string& ev) override {}

	void SetParent(Transform* parent) { m_parent = parent; }
	Transform* GetParent() const { return m_parent; }

	XMVECTOR GetPosition() const { return m_position; }
	float GetRotation() const { return m_rotationRad; }
	XMVECTOR GetScale() const { return m_scale; }

	void OnMoveMessage(void* data);
	void OnRotateMessage(void* data);
	void OnScaleMessage(void* data);

	void SetPosition(XMVECTOR pos);
	void SetRotation(float rad);
	void SetScale(XMVECTOR pos);


	void Translate(XMVECTOR delta);
	void Rotate(float deltaAngleRad);
	void Scale(XMVECTOR deltaScale);

	XMMATRIX GetWorldMatrix() const;
};
