#pragma once
#include "Object.h"
#include "Transform.h"
#include "BitmapRender.h"

class CreateObject
{
public:
	enum direction
	{
		all = 0,
		left = 1,
		right = 2,
		up = 3,
		down = 4
	};

	static void CreateObjectsOutOfScreen
	(
		vector<unique_ptr<Object>>& objects,
		string imageKey,
		float screenWidth, float screenHeight,
		float width, float height,
		int count,
		float speed = 1.0f,
		direction exclusiveDirection = direction::all
	);
};

class NewObject : public Object
{
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;
	XMVECTOR m_destination = { 0.0f, 0.0f, 0.0f, 1.0f };
	float m_speedPerSec = 0.0f;

public:
	NewObject(float posX, float posY, float width, float height, float rotation, const std::string& bitmapKey = "Sample.png", float speed = 1.0f, XMVECTOR destination = { 0.0f, 0.0f, 0.0f, 1.0f }, int order = 0);

	void FixedUpdate(double deltaTime) override { Object::FixedUpdate(deltaTime); }
	void Update(double deltaTime) override;
	void LateUpdate(double deltaTime) override { Object::LateUpdate(deltaTime); }
	void Render(Renderer& renderer) override { Object::Render(renderer); }

	virtual ~NewObject() = default;
};

class OneTimeEffect : public Object
{
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;

public:
	OneTimeEffect(float posX, float posY, float width, float height, const std::string& bitmapKey = "Sample.png", int order = 0)
	{
		m_transform = AddComponent<Transform>();
		m_bitmapRender = AddComponent<BitmapRender3D>(bitmapKey, width, height);
		m_bitmapRender->SetOrder(order);
		m_bitmapRender->SetActive(true);
		m_transform->SetPosition(DirectX::XMVectorSet(posX, posY, 0.0f, 1.0f));
		m_transform->SetScale(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	}

	bool m_isDead = false; // 효과가 끝났는지 여부

	void Update(double deltaTime) override { m_bitmapRender->Update(deltaTime);	m_isDead = m_bitmapRender->IsEnded(); }
	void Render(Renderer& renderer) override { m_bitmapRender->Render(renderer); }

	virtual ~OneTimeEffect() = default;
};