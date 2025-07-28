#include "pch.h"
#include "BitmapRender.h"
#include "Transform.h"

bool BitmapRender::Extension(const string& bitmapKey, const string& ext) const
{
	if (bitmapKey.length() >= ext.length())
	{
		return (bitmapKey.compare(bitmapKey.length() - ext.length(), ext.length(), ext) == 0);
	}
	else return false;
}

void BitmapRender::Render(Renderer& renderer)
{
	if (!m_isActive || !m_transform) { std::cout << "그릴 준비안됨" << std::endl; return; }
	if (m_isAnimated) RenderAnimation(renderer);
	else RenderBitmap(renderer);
}

void BitmapRender::RenderAnimation(Renderer& renderer)
{
	if (!m_animator.IsValid()) { std::cout << "그릴 준비안됨" << std::endl; return; }

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

void BitmapRender::RenderBitmap(Renderer& renderer)
{
	XMVECTOR worldPosition = m_transform->GetPosition();
	XMVECTOR worldScale = m_transform->GetScale();

	float objWorldX = XMVectorGetX(worldPosition);
	float objWorldY = XMVectorGetY(worldPosition);

	D2D1_RECT_F destRect = D2D1::RectF
	(
		objWorldX /*- (m_width * XMVectorGetX(worldScale) / 2.0f)*/,
		objWorldY /*- (m_height * XMVectorGetY(worldScale) / 2.0f)*/,
		objWorldX + (m_width * XMVectorGetX(worldScale) /*/ 2.0f*/),
		objWorldY + (m_height * XMVectorGetY(worldScale) /*/ 2.0f*/)
	);

	renderer.DrawBitmap
	(
		m_bitmap.Get(),
		destRect
	);
}
