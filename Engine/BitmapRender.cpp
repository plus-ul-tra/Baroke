#include "pch.h"
#include "BitmapRender.h"
#include "Transform.h"

bool BitmapRender::Extension(const string& bitmapKey, const string& ext) const
{
	if (bitmapKey.length() >= ext.length())
	{
		return (bitmapKey.compare(bitmapKey.length() - ext.length(), ext.length(), ext) == 0);
	}
	return false;
}

void BitmapRender::Update(double deltaTime)
{
	if (m_isAnimated)
	{
		m_animator.Update(static_cast<float>(deltaTime) * 1000.0f);
	}
}

void BitmapRender::Render(Renderer& renderer)
{
	if (!m_isActive || !m_transform) { return; }

	XMVECTOR worldPosition = m_transform->GetPosition();
	XMVECTOR worldScale = m_transform->GetScale();

	float objWorldX = XMVectorGetX(worldPosition);
	float objWorldY = XMVectorGetY(worldPosition);

	D2D1_RECT_F destRect = D2D1::RectF
	(
		objWorldX - (m_width * XMVectorGetX(worldScale) / 2.0f),
		objWorldY - (m_height * XMVectorGetY(worldScale) / 2.0f),
		objWorldX + (m_width * XMVectorGetX(worldScale) / 2.0f),
		objWorldY + (m_height * XMVectorGetY(worldScale) / 2.0f)
	);

	if (m_isAnimated || !m_animator.IsValid())
	{
		const Frame& frame = m_animator.GetCurrentFrame();
		renderer.DrawBitmap
		(
			m_animator.GetCurrentClip()->GetBitmap(),
			destRect,
			frame.ToRectF()
		);
	}
	else if (!m_isAnimated)
	{
		renderer.DrawBitmap
		(
			m_bitmap.Get(),
			destRect
		);
	}
	else std::cout << "유효한 애니메이션 클립이나 비트맵이 없습니다." << std::endl;
}