#pragma once
#include "BaseRender.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace DirectX;
enum class ShapeType
{
    Rectangle,
    Circle
};

class GeoRender : public BaseRender {
private:

    ShapeType m_shapeType; // ���� Ÿ��

    // ���� ����
    float m_width;
    float m_height;
    float m_radius;

    DirectX::XMFLOAT4 m_color;
public:
    // �簢��
    GeoRender(float width, float height, XMFLOAT4 color,
        ShapeType type = ShapeType::Rectangle);
    // ��
    GeoRender(float radius, DirectX::XMFLOAT4 color,
        ShapeType type = ShapeType::Circle);
    virtual ~GeoRender() = default; 

    void Render(Renderer& renderer) override;
    ShapeType GetShapeType() const { return m_shapeType; }

    void SetRectangleSize(float width, float height);
    void SetCircleRadius(float radius);
    void SetColor(XMFLOAT4 color) { m_color = color; }
    


};