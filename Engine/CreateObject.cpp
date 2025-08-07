#include "pch.h"
#include "CreateObject.h"

NewObject::NewObject(float posX, float posY, float width, float height, const std::string& bitmapKey, float speed, int order)
{
	m_transform = AddComponent<Transform>();
	m_bitmapRender = AddComponent<BitmapRender3D>(bitmapKey, width, height);
	m_bitmapRender->SetOrder(order);
	m_bitmapRender->SetActive(true);

	m_transform->SetPosition(DirectX::XMVectorSet(posX, posY, 0.0f, 1.0f));
	m_transform->SetScale(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	m_transform->SetRotation(0.0f);

	m_speedPerSec = speed;
}