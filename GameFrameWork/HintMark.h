#pragma once
#include "Engine.h"

class HintMark : public Object
{
public:
	HintMark(int w, int h, const std::string& texKey, int order = 100)
	{
		m_transform = AddComponent<Transform>();
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_bitmapRender = AddComponent<BitmapRender3D>(texKey, w, h);  // 시그니처에 맞게
		m_bitmapRender->SetOrder(order);
		m_bitmapRender->SetActive(false);
	}

	void SetScreenPos(int x, int y)
	{
		if (m_transform) m_transform->SetPosition(DirectX::XMVectorSet(x, y, 0.f, 1.f));
	}

	void Show(bool v)
	{
		if (m_bitmapRender) m_bitmapRender->SetActive(v);
	}

	void OnEvent(const std::string& ev) {}
	void Update(double deltaTime) {}

private:
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;
};
