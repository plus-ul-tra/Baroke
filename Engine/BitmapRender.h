#pragma once
#include "BaseRender.h"
#include "SpriteManager.h"

class BitmapRender : public BaseRender
{
	float m_width;
	float m_height;
	SpriteAnimator m_animator;

public:
	BitmapRender(const std::wstring& animationKey, float width, float height) : m_width(width), m_height(height)
	{ m_animator.SetClips(&SpriteManager::GetInstance().GetAnimationClips(animationKey)); }

	void Render(Renderer& renderer) override;

	void SetAnimator(const std::wstring& animationKey) { m_animator.SetClips(&SpriteManager::GetInstance().GetAnimationClips(animationKey)); }
	SpriteAnimator& GetAnimator() { return m_animator; }

	void SetWidth(float width) { m_width = width; }
	float GetWidth() const { return m_width; }
	void SetHeight(float height) { m_height = height; }
	float GetHeight() const { return m_height; }

	virtual ~BitmapRender() = default;
};