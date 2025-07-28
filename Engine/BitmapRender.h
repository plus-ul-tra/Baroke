#pragma once
#include "BaseRender.h"
#include "SpriteManager.h"

class BitmapRender : public BaseRender
{
	float m_width;
	float m_height;

	bool m_isAnimated = false;
	SpriteAnimator m_animator;
	ComPtr<ID2D1Bitmap1> m_bitmap = nullptr;

public:
	BitmapRender(const string& bitmapKey, float width, float height, bool isAnimation) : m_width(width), m_height(height), m_isAnimated(isAnimation)
	{
		if(isAnimation) m_animator.SetClips(&SpriteManager::GetInstance().GetAnimationClips(bitmapKey));
		else m_bitmap = SpriteManager::GetInstance().GetTexture(bitmapKey);
	}

	void Render(Renderer& renderer) override;
	void RenderAnimation(Renderer& renderer);
	void RenderBitmap(Renderer& renderer);

	void SetAnimator(const string& animationKey) { m_animator.SetClips(&SpriteManager::GetInstance().GetAnimationClips(animationKey)); }
	SpriteAnimator& GetAnimator() { return m_animator; }

	void SetWidth(float width) { m_width = width; }
	float GetWidth() const { return m_width; }
	void SetHeight(float height) { m_height = height; }
	float GetHeight() const { return m_height; }

	virtual ~BitmapRender() = default;
};