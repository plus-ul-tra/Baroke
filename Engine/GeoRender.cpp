#include "pch.h"
#include "GeoRender.h"
#include "Transform.h"
#include "Renderer.h"

GeoRender::GeoRender(float width, float height, XMFLOAT4 color, ShapeType type) :
    m_width(width), m_height(height), m_color(color), m_shapeType(type), m_radius(0.0f)
{

}

GeoRender::GeoRender(float radius, XMFLOAT4 color, ShapeType type) :
    m_radius(radius), m_color(color), m_shapeType(type), m_width(0.0f), m_height(0.0f)
{

}

void GeoRender::Render(Renderer& renderer)
{
    if (!m_isActive || !m_transform)
    {
        std::cout << "그릴 준비안됨" << std::endl;
        return;
    }


    XMVECTOR worlPosition = m_transform->GetPosition();
    XMVECTOR worldScale = m_transform->GetScale();

    float objWorldX = XMVectorGetX(worlPosition);
    float objWorldY = XMVectorGetY(worlPosition);



    if (m_shapeType == ShapeType::Rectangle) {

        float finalWidth = m_width * DirectX::XMVectorGetX(worldScale);
        float finalHeight = m_height * DirectX::XMVectorGetY(worldScale);

        // 사각형 왼쪽 상단 좌표.
        float drawX = objWorldX - (finalWidth / 2.0f);
        float drawY = objWorldY - (finalHeight / 2.0f);

        //D2D1_RECT_F rect = D2D1::RectF(drawX, drawY, drawX + finalWidth, drawY + finalHeight);

        // TODO: 회전 처리는 Renderer::DrawRectangle 함수에서 Direct2D의 Transform 적용을 통해 구현해야 합니다.
        // 예를 들어, renderer->SetTransform(D2D1_MATRIX_3X2_F); 후 그리기 Why?

        renderer.DrawRect(drawX, drawY, drawX + finalWidth, drawY + finalHeight, 
                            D2D1::ColorF(m_color.x, m_color.y, m_color.z, m_color.w)); //컬러 기본값 설정 필요 
    }
    else if (m_shapeType == ShapeType::Circle) {
        
        float finalRadius = m_radius * (XMVectorGetX(worldScale) + XMVectorGetY(worldScale)) / 2.0f;

        renderer.DrawCircle(objWorldX, objWorldY,finalRadius, 
                  D2D1::ColorF(m_color.x, m_color.y, m_color.z, m_color.w));
    }
}

void GeoRender::SetRectangleSize(float width, float height)
{
    if (m_shapeType == ShapeType::Rectangle)
    {
        m_width = width;
        m_height = height;
    }
}

void GeoRender::SetCircleRadius(float radius)
{
    if (m_shapeType == ShapeType::Circle)
    {
        m_radius = radius;
    }
}
