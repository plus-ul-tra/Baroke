#include"pch.h"
#include "Object.h"


void Object::Update(double deltaTime)
{
	// 오브젝트 하나가 가지고 있는 컴포넌트들
	for (auto& com : m_Components)
	{
		com->Update(deltaTime);
	}
}


void Object::FixedUpdate(double deltaTime)
{
	// 오브젝트 하나가 가지고 있는 컴포넌트들
	for (auto& com : m_Components)
	{
		com->FixedUpdate(deltaTime);
	}
}


void Object::LateUpdate(double deltaTime)
{
	// 오브젝트 하나가 가지고 있는 컴포넌트들
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
