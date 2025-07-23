#include"pch.h"
#include "Object.h"


void Object::Update(double deltaTime)
{
	// ������Ʈ �ϳ��� ������ �ִ� ������Ʈ��
	for (auto& com : m_Components)
	{
		com->Update(deltaTime);
	}
}


void Object::FixedUpdate(double deltaTime)
{
	// ������Ʈ �ϳ��� ������ �ִ� ������Ʈ��
	for (auto& com : m_Components)
	{
		com->FixedUpdate(deltaTime);
	}
}


void Object::LateUpdate(double deltaTime)
{
	// ������Ʈ �ϳ��� ������ �ִ� ������Ʈ��
	for (auto& com : m_Components)
	{
		com->LateUpdate(deltaTime);
	}
}

void Object::_SendMessage(const mycore::MessageID msg, void* data)
{
	for (auto& com : m_Components)
	{
		com->HandleMessage(msg, data);
	}
}


void Object::SendEvent(const std::string& ev)
{
	for (auto& com : m_Components)
	{
		com->OnEvent(ev);
	}
}
