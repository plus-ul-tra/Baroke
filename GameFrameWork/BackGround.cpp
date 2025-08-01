#include "cpch.h"
#include "BackGround.h"

using namespace DirectX;
using namespace std;

BackGround::BackGround(float posX, float posY, float width, float height)
{
	m_transform = AddComponent<Transform>();
	m_bitmapRender = AddComponent<BitmapRender3D>("testBack2.png", width, height);
	m_transform->SetPosition(XMVectorSet(posX, posY, 0.0f, 1.0f));
	m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	m_transform->SetRotation(0.0f);

	m_bitmapRender->SetActive(true);
}
