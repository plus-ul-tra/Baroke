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