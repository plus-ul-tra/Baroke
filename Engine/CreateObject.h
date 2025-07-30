#pragma once
#include "Engine.h"

class NewObject : public Object
{
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;
	float m_speedPerSec = 0.0f;

public:
	NewObject(float posX, float posY, float width, float height, const std::string& bitmapKey = "Sample.png", int order = 0, float speed = 500.0f);

	void FixedUpdate(double deltaTime) override { Object::FixedUpdate(deltaTime); }
	void Update(double deltaTime) override { Object::Update(deltaTime); }
	void LateUpdate(double deltaTime) override { Object::LateUpdate(deltaTime); }

	void Move(float dx, float dy);
	void Move(DirectX::XMVECTOR direction, double deltaTime);

	virtual ~NewObject() = default;
};