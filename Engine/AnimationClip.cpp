#include "pch.h"
#include "AnimationClip.h"

void SpriteAnimator::SetCurrentClip(unsigned int index)
{
	if (m_clips && !m_clips->empty() && index < m_clips->size())
	{
		m_clip = (*m_clips)[index].second;
		clipIndex = index;
	}
	else throw std::runtime_error("해당 인댁스의의 애니메이션 클립을 찾을 수 없음.");
}

void SpriteAnimator::SetCurrentClip(const string& name)
{
	if (!m_clips || m_clips->empty()) throw std::runtime_error("애니메이션 클립이 비어있음.");

	auto it = find_if(m_clips->begin(), m_clips->end(), [&name](const pair<string, AnimationClip>& clip) { return clip.first == name; });
	if (it != m_clips->end())
	{
		m_clip = it->second;
		clipIndex = static_cast<unsigned int>(distance(m_clips->begin(), it));
	}
	else throw std::runtime_error("해당 이름의 애니메이션 클립을 찾을 수 없음.");
}

const AnimationClip* SpriteAnimator::GetCurrentClip() const
{
	if (m_clips && clipIndex < m_clips->size())
	{
		return &(*m_clips)[clipIndex].second;
	}
	return nullptr;
}

void SpriteAnimator::Update(float deltaTime)
{
	if (m_clip.GetFrames().empty()) throw std::runtime_error("애니메이션 클립이 유효하지 않거나 프레임이 없음.");

	m_elapsedTime += deltaTime;
	float totalDuration = m_clip.GetTotalDuration();

	if (m_isLoop)
	{
		if (m_elapsedTime >= totalDuration) m_elapsedTime = fmod(m_elapsedTime, totalDuration);
	}
	else if (m_elapsedTime > totalDuration)
	{
		m_elapsedTime = totalDuration;
	}
}

const Frame& SpriteAnimator::GetCurrentFrame() const
{
	static Frame fallbackDummy = { {0,0,0,0}, 0.0f };
	if (m_clip.GetFrames().empty()) return fallbackDummy;

	float accumulatedTime = 0.0f;
	for (const auto& frame : m_clip.GetFrames())
	{
		accumulatedTime += frame.duration;

		if (m_elapsedTime < accumulatedTime) return frame;
	}

	return m_clip.GetFrames().back();
}