#pragma once
#include "pch.h"
#include "BaseRender.h"
#include "Transform.h"
#include "Object.h"

BaseRender::BaseRender() : m_isActive(false), // �⺻������ ��Ȱ��ȭ ����
						   m_order(0)
{

}

// override;
void BaseRender::SetOwner(Object* owner)
{
	Component::SetOwner(owner); //m_owner = owner;

	// BaseRender�� �Ļ�Ŭ�����鿡 ���� Transform ���� ����
	if (m_owner) {
		std::cout << "Render�� Transform ���� ����" << std::endl;
		m_transform = m_owner->GetComponent<Transform>();
	}
	else {
		m_transform = nullptr;
	}

}
