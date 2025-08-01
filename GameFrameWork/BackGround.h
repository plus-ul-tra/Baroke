#pragma once
#include "Engine.h"

class BackGround : public Object
{
private:
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;
public:
	BackGround(float posX, float PosY, float width, float height);
	~BackGround() = default;

	virtual void Update(double deltaTime) override {};


	Transform* GetTransform() { return m_transform; }
	BitmapRender3D* GetBitmapRender() { return m_bitmapRender; }
};