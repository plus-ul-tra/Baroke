#pragma once
#include "pch.h"
#include "BaseRender.h"
#include "Transform.h"
#include "Object.h"

BaseRender::BaseRender() : m_isActive(false), // 기본적으로 비활성화 상태
						   m_order(0)
{

}

// override;
void BaseRender::SetOwner(Object* owner)
{
	Component::SetOwner(owner); //m_owner = owner;

	// BaseRender의 파생클래스들에 대한 Transform 연결 보장
	if (m_owner) {
		m_transform = m_owner->GetComponent<Transform>();
	}
	else {
		m_transform = nullptr;
	}

}
