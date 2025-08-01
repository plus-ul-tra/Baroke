#pragma once
#include "BaseRender.h"
#include "Collider2D.h"
#include <DirectXMath.h>


using namespace DirectX;


class GeoRender : public BaseRender {
private:
    Collider2D* m_collider2D = nullptr;
    void SetCollider2D();
public:

    GeoRender() = default;
    virtual ~GeoRender() = default;
    void SetOwner(Object* owner) override;
    void Render(Renderer& renderer , const string& mode) override;
};