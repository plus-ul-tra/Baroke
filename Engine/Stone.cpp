#include "pch.h"
#include "Stone.h"
#include "SceneManager.h"

unordered_map<StoneAbility, JokerStoneInfo> m_jokerInfoMap;
unordered_map<StoneAbility, string> m_jokerAnimationMap;

void Stone::Update(double deltaTime)
{
	if (m_sprite && m_sprite->IsActive()) m_sprite->Update(deltaTime);

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
		if (ability == StoneAbility::jokerBlackhole) SceneManager::GetInstance().ChangePostProcessing("CRTFilter"); // 조커 블랙홀 능력 해제시 포스트 프로세싱 초기화
		m_queueRemoveTime -= deltaTime;
		if (m_queueRemoveTime <= 0.0)
		{
			m_sprite->SetActive(false);
			m_isRemoved = true;
			m_isRemoving = false;

			SoundManager::GetInstance().PlaySoundOnce("stoneDeath.wav");
		}
	}

	// 착수 이펙트
	if (m_placeEffect)
	{
		m_placeEffect->Update(deltaTime);
		if (m_placeEffect->m_isDead) m_placeEffect = nullptr; // 착수 이펙트가 끝나면 제거
	}

	// 제거 이펙트
	if (m_isRemoved && m_removeEffect)
	{
		m_removeEffect->Update(deltaTime);
		if (m_removeEffect->m_isDead) m_removeEffect = nullptr; // 제거 이펙트가 끝나면 제거
	}
}

void Stone::Render(Renderer& renderer)
{
	if (m_placeEffect) m_placeEffect->Render(renderer);
	if (m_isRemoved && m_removeEffect) m_removeEffect->Render(renderer);
}

void Stone::Move(POINT position, double duration)
{
	m_lerpTime = duration;

	m_lerpStartPosition = m_transform->GetPosition();
	m_lerpEndPosition = XMVectorSet(static_cast<float>(position.x) + m_size / 2, static_cast<float>(position.y) + m_size / 2, 0.0f, 1.0f);
}

void Stone::DeathMove(double duration)
{
	m_lerpTime = duration;
	m_lerpStartPosition = m_transform->GetPosition();
	m_lerpEndPosition = m_removePosition; // 제거 위치로 이동
	m_lerpElapsedTime = 0.0;

	m_isRemoving = true;
	m_queueRemoveTime = duration;

	m_removeEffect = make_unique<OneTimeEffect>(static_cast<float>(XMVectorGetX(m_removePosition)), static_cast<float>(XMVectorGetY(m_removePosition)), m_size - m_offset, m_size - m_offset, "sit_pattern1_sheet.json");
}

POINT Stone::GetPosition() const
{
	XMVECTOR pos = m_transform->GetPosition();
	return { static_cast<int>(XMVectorGetX(pos)), static_cast<int>(XMVectorGetY(pos)) };
}

void Stone::ChangeColor(bool isBlack)
{

	if (isBlack) m_sprite->ChangeTexture("T_Blackstone.png");
	else m_sprite->ChangeTexture("T_whitestone.png");
}

void JokerStone::UpdateAbility(StoneAbility newAb)
{
	if (ability == newAb) return;        

	ability = newAb;                  
	m_jokerInfo = m_jokerInfoMap[newAb];  
	m_sprite->ChangeTexture(m_jokerInfo.fileName);
}
