#pragma once
#include "Engine.h"

class Player : public Object {
private:

	Transform* m_transform = nullptr;
	GeoRender* m_geoRender = nullptr;
	Collider2D* m_Collider = nullptr;
public:
	Player(float posX, float posY, float width, float height, DirectX::XMFLOAT4 color);
	virtual ~Player() = default;

	void FixedUpdate(double deltaTime) override;
	void Update(double deltaTime) override;
	void LateUpdate(double deltaTime) override;

	Transform* GetTransform() { return m_transform; }
	Collider2D* GetCollider() { return m_Collider; }
	GeoRender* GetGeoRender() { return m_geoRender; }


};