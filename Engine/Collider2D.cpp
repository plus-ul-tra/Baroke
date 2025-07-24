#pragma once
#include"pch.h"
#include "Collider2D.h"
#include "Object.h"

void Collider2D::GetTransform()
{
	if (m_owner) {
		//std::cout << "Collider Transform 완료" << std::endl;
		m_transform = m_owner->GetComponent<Transform>();
		m_posX = DirectX::XMVectorGetX(m_transform->GetPosition());
		m_posY = DirectX::XMVectorGetY(m_transform->GetPosition());
	}
	else {
		m_transform = nullptr;
	}
}

Collider2D::Collider2D()
{
	//std::cout << "Collider생성완료" << std::endl;
	//GetTransform();
}

// 사각형
void Collider2D::SetSize(float width, float height)
{
	if (m_type == ColliderType::Rectangle) {
		m_width = width;
		m_height = height;
	}
	else if(m_type == ColliderType::Circle){
		m_radius = (width + height) / 2;
	}
}

// Set Owner 중요
void Collider2D::SetOwner(Object* owner)
{
	Component::SetOwner(owner);
	//std::cout << "Collider owner 설정완료" << std::endl;
	GetTransform();
}

void Collider2D::FixedUpdate(double deltaTime)
{

}
