#include "pch.h"
#include "CreateObject.h"

NewObject::NewObject(float posX, float posY, float width, float height, const std::string& bitmapKey, int order, float speed)
{
	m_transform = AddComponent<Transform>();
	m_bitmapRender = AddComponent<BitmapRender>(bitmapKey, width, height);
	m_bitmapRender->SetOrder(order);
	m_bitmapRender->SetActive(true);

	m_transform->SetPosition(DirectX::XMVectorSet(posX, posY, 0.0f, 1.0f));
	m_transform->SetScale(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	m_transform->SetRotation(0.0f);

	m_speedPerSec = speed;
}


void NewObject::Move(float dx, float dy)
{
	float distance = m_speedPerSec;
	auto pos = m_transform->GetPosition();
	pos = XMVectorAdd(pos, XMVectorSet(dx * distance, dy * distance, 0, 0));
	m_transform->SetPosition(pos);
}


void NewObject::Move(DirectX::XMVECTOR direction, double deltaTime)
{
	float distance = static_cast<float>(m_speedPerSec * deltaTime);
	auto pos = m_transform->GetPosition();
	pos = XMVectorAdd(pos, XMVectorScale(direction, distance));

	m_transform->SetPosition(pos);
}