#pragma once
#include "Engine.h"

class Player : public Object {
private:

	Transform* m_transform = nullptr;
	GeoRender* m_geoRender = nullptr;
	BitmapRender* m_bitmapRender = nullptr;

public:
	Player(float x, float y, float width, float height, DirectX::XMFLOAT4 color);
	virtual ~Player() = default;
	void FixedUpdate(double deltaTime) override;
	void Update(double deltaTime) override;
	void LateUpdate(double deltaTime) override;

	Transform* GetTransform() { return m_transform; }
	GeoRender* GetGeoRender() { return m_geoRender; }
	BitmapRender* GetBitmapRender() { return m_bitmapRender; }


};