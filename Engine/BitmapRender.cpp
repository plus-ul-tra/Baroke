#include "pch.h"
#include "BitmapRender.h"
#include "Transform.h"

void BitmapRender::Render(Renderer& renderer)
{
	if (!m_isActive || !m_transform || !m_animator.IsValid()) { std::cout << "�׸� �غ�ȵ�" << std::endl; return; }

	XMVECTOR worldPosition = m_transform->GetPosition();
	XMVECTOR worldScale = m_transform->GetScale();

	float objWorldX = XMVectorGetX(worldPosition);
	float objWorldY = XMVectorGetY(worldPosition);

	const Frame& frame = m_animator.GetCurrentFrame();
	D2D1_RECT_F destRect = D2D1::RectF
	(
		objWorldX - (m_width * XMVectorGetX(worldScale) / 2.0f),
		objWorldY - (m_height * XMVectorGetY(worldScale) / 2.0f),
		objWorldX + (m_width * XMVectorGetX(worldScale) / 2.0f),
		objWorldY + (m_height * XMVectorGetY(worldScale) / 2.0f)
	);

	m_animator.SetCurrentClip(1);
	m_animator.Update(5.0f);
	renderer.DrawBitmap
	(
		m_animator.GetCurrentClip()->GetBitmap(),
		destRect,
		frame.ToRectF()
	);
}