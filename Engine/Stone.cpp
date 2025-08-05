#include "pch.h"
#include "Stone.h"
#include "SceneManager.h"

unordered_map<StoneAbility, JokerStoneInfo> m_jokerInfoMap;

void Stone::Update(double deltaTime)
{
	if (m_lerpTime > 0.0)
	{
		m_lerpElapsedTime += deltaTime;
		m_transform->SetPosition(XMVectorLerp(m_lerpStartPosition, m_lerpEndPosition, static_cast<float>(m_lerpElapsedTime / m_lerpTime)));

		if (m_lerpElapsedTime >= m_lerpTime)
		{
			m_transform->SetPosition(m_lerpEndPosition);
			m_lerpTime = 0.0;
			m_lerpElapsedTime = 0.0;
		}
	}
	if (m_isRemoving)
	{
		m_queueRemoveTime -= deltaTime;
		if (m_queueRemoveTime <= 0.0)
		{
			SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
			m_sprite->SetActive(false);
			m_isRemoved = true;
		}
	}
}

void Stone::Move(POINT position, double duration)
{
	m_lerpTime = duration;

	m_lerpStartPosition = m_transform->GetPosition();
	m_lerpEndPosition = XMVectorSet(static_cast<float>(position.x) + m_size / 2, static_cast<float>(position.y) + m_size / 2, 0.0f, 1.0f);
}

POINT Stone::GetPosition() const
{
	XMVECTOR pos = m_transform->GetPosition();
	return { static_cast<int>(XMVectorGetX(pos)), static_cast<int>(XMVectorGetY(pos)) };
}