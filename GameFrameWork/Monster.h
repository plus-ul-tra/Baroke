#pragma once
#include "Engine.h"
class Monster : public Object {
private:

	Transform* m_transform = nullptr;
	GeoRender* m_geoRender = nullptr;

public:
	Monster(float x, float y, float width, float height, DirectX::XMFLOAT4 color);
	virtual ~Monster() = default;

	void Update(double deltaTime) override;

	Transform* GetTransform() { return m_transform; }
	GeoRender* GetGeoRender() { return m_geoRender; }


};