#pragma once
#include "Engine.h"

class Player : public Object {
private:

	Transform* m_transform = nullptr;
	GeoRender* m_geoRender = nullptr;
	BitmapRender* m_bitmapRender = nullptr;
	TextRender* m_textRender = nullptr;
	Collider2D* m_Collider = nullptr;


	float m_speedPerSec = 500.f;


public:
	Player(float posX, float posY, float width, float height, DirectX::XMFLOAT4 color);
	virtual ~Player() = default;

	void FixedUpdate(double deltaTime) override;
	void Update(double deltaTime) override;
	void LateUpdate(double deltaTime) override;

	void Move(float dx, float dy);
	void Move(DirectX::XMVECTOR direction, double deltaTime);
// 	void MoveUp();
// 	void MoveLeft();
// 	void MoveDown();
// 	void MoveRight();



	Transform* GetTransform() { return m_transform; }
	Collider2D* GetCollider() { return m_Collider; }
	GeoRender* GetGeoRender() { return m_geoRender; }
	BitmapRender* GetBitmapRender() { return m_bitmapRender; }


};