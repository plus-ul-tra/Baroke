#pragma once
#include "BaseRender.h"
#include "SpriteManager.h"

class BitmapRender : public BaseRender
{
	float m_width;
	float m_height;

	SpriteAnimator m_animator;
	ComPtr<ID2D1Bitmap1> m_bitmap = nullptr;

	bool m_isAnimated = false;
	bool Extension(const string& bitmapKey, const string& ext = ".json") const;

public:
	BitmapRender(const string& bitmapKey, float width, float height) : m_width(width), m_height(height)
	{
		if (Extension(bitmapKey, ".json"))
		{
			m_animator.SetClips(&SpriteManager::GetInstance().GetAnimationClips(bitmapKey));
			m_animator.SetCurrentClip(0);
			m_isAnimated = true;
		}
		else if (Extension(bitmapKey, ".png"))
		{
			m_bitmap = SpriteManager::GetInstance().GetTexture(bitmapKey);
			m_isAnimated = false;
		}
	}

	void Update(double deltaTime) override;
	void Render(Renderer& renderer) override;

	void SetAnimator(const string& animationKey) { m_animator.SetClips(&SpriteManager::GetInstance().GetAnimationClips(animationKey)); }
	SpriteAnimator& GetAnimator() { return m_animator; }

	void SetWidth(float width) { m_width = width; }
	float GetWidth() const { return m_width; }
	void SetHeight(float height) { m_height = height; }
	float GetHeight() const { return m_height; }

	virtual ~BitmapRender() = default;
};