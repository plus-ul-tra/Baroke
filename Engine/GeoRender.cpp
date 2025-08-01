#include "pch.h"
#include "GeoRender.h"
#include "Transform.h"
#include "Collider2D.h"
#include "Renderer.h"
#include "Object.h"


void GeoRender::SetCollider2D()
{
	
    // SetOwner�� ��� �̹� BaseRender���� ����
    if(m_owner){
     m_collider2D = m_owner->GetComponent<Collider2D>();
     //std::cout << "collider ���Ἲ��" << std::endl;
	}
	else {
        m_collider2D = nullptr; // std::cout << "collider �������" << std::endl;
	}
}

//-----------------------------------------------
//public


void GeoRender::SetOwner(Object* owner)
{
    BaseRender::SetOwner(owner); //-> component::SetOwner ȣ�� �� Transform ����
    SetCollider2D();
}

void GeoRender::Render(Renderer& renderer)
{
       
	if (!m_isActive || !m_collider2D)
	{
		//std::cout << "�׸� �غ�ȵ�" << std::endl;
		return;
	}


    XMVECTOR worlPosition = m_transform->GetPosition();
    XMVECTOR worldScale = m_transform->GetScale(); 

    float objWorldX = XMVectorGetX(worlPosition);
    float objWorldY = XMVectorGetY(worlPosition);

    if (m_collider2D->GetColliderType() == ColliderType::Rectangle) {

        float finalWidth = m_collider2D->GetWidth() * DirectX::XMVectorGetX(worldScale);
        float finalHeight = m_collider2D->GetHeight() * DirectX::XMVectorGetY(worldScale);

        // �簢�� ���� ��� ��ǥ.
        float drawX = objWorldX - (finalWidth / 2.0f);
        float drawY = objWorldY - (finalHeight / 2.0f);

        //D2D1_RECT_F rect = D2D1::RectF(drawX, drawY, drawX + finalWidth, drawY + finalHeight);

        // TODO: ȸ�� ó���� Renderer::DrawRectangle �Լ����� Direct2D�� Transform ������ ���� �����ؾ� �մϴ�.
        // ���� ���, renderer->SetTransform(D2D1_MATRIX_3X2_F); �� �׸��� Why?

     //   renderer.DrawRect(drawX, drawY, drawX + finalWidth, drawY + finalHeight, 
     //       m_collider2D->GetColor());
    }
    else if (m_collider2D->GetColliderType() == ColliderType::Circle) {
        
        float finalRadius = m_collider2D->GetRadius() * (XMVectorGetX(worldScale) + XMVectorGetY(worldScale)) / 2.0f;

		// renderer.DrawCircle(objWorldX, objWorldY,finalRadius, 
		//           m_collider2D->GetColor());
    }
}

