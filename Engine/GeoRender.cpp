#include "pch.h"
#include "GeoRender.h"
#include "Transform.h"
#include "Collider2D.h"
#include "Renderer.h"
#include "Object.h"


void GeoRender::SetCollider2D()
{
	
    // SetOwner의 경우 이미 BaseRender에서 했음
    if(m_owner){
     m_collider2D = m_owner->GetComponent<Collider2D>();
     //std::cout << "collider 연결성공" << std::endl;
	}
	else {
        m_collider2D = nullptr; // std::cout << "collider 연결실패" << std::endl;
	}
}

//-----------------------------------------------
//public


void GeoRender::SetOwner(Object* owner)
{
    BaseRender::SetOwner(owner); //-> component::SetOwner 호출 및 Transform 연결
    SetCollider2D();
}

void GeoRender::Render(Renderer& renderer)
{
       
	if (!m_isActive || !m_collider2D)
	{
		//std::cout << "그릴 준비안됨" << std::endl;
		return;
	}


    XMVECTOR worlPosition = m_transform->GetPosition();
    XMVECTOR worldScale = m_transform->GetScale(); 

    float objWorldX = XMVectorGetX(worlPosition);
    float objWorldY = XMVectorGetY(worlPosition);

    if (m_collider2D->GetColliderType() == ColliderType::Rectangle) {

        float finalWidth = m_collider2D->GetWidth() * DirectX::XMVectorGetX(worldScale);
        float finalHeight = m_collider2D->GetHeight() * DirectX::XMVectorGetY(worldScale);

        // 사각형 왼쪽 상단 좌표.
        float drawX = objWorldX - (finalWidth / 2.0f);
        float drawY = objWorldY - (finalHeight / 2.0f);

        //D2D1_RECT_F rect = D2D1::RectF(drawX, drawY, drawX + finalWidth, drawY + finalHeight);

        // TODO: 회전 처리는 Renderer::DrawRectangle 함수에서 Direct2D의 Transform 적용을 통해 구현해야 합니다.
        // 예를 들어, renderer->SetTransform(D2D1_MATRIX_3X2_F); 후 그리기 Why?

     //   renderer.DrawRect(drawX, drawY, drawX + finalWidth, drawY + finalHeight, 
     //       m_collider2D->GetColor());
    }
    else if (m_collider2D->GetColliderType() == ColliderType::Circle) {
        
        float finalRadius = m_collider2D->GetRadius() * (XMVectorGetX(worldScale) + XMVectorGetY(worldScale)) / 2.0f;

		// renderer.DrawCircle(objWorldX, objWorldY,finalRadius, 
		//           m_collider2D->GetColor());
    }
}

