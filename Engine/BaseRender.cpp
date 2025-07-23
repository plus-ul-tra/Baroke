#pragma once
#include "pch.h"
#include "BaseRender.h"
#include "Transform.h"
#include "Object.h"

BaseRender::BaseRender() : m_isActive(false), // 기본적으로 비활성화 상태
						   m_order(0)
{

}

void BaseRender::SetOwner(Object* owner)
{
	Component::SetOwner(owner);

	if (m_owner) {
		m_transform = m_owner->GetComponent<Transform>();
	}
	else {
		m_transform = nullptr;
	}

}
