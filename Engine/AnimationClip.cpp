#include "pch.h"
#include "AnimationClip.h"

void SpriteAnimator::SetCurrentClip(unsigned int index)
{
	if (!m_clipsMap || m_clipsMap->empty()) {
		OutputDebugStringA("ERROR: SpriteAnimator::SetCurrentClip(index) - No animation clips set or map is empty.\n");
		return;
	}
	if (index >= m_clipsMap->size()) {
		OutputDebugStringA("ERROR: SpriteAnimator::SetCurrentClip(index) - Index out of bounds.\n");
		
		return;
	}

	auto it = m_clipsMap->begin();
	std::advance(it, index);
	m_currentClip = it->second;
	m_elapsedTime = 0.0f;
	m_currentFrameIndex = 0;
	m_isLoop = m_currentClip.IsLoop();
}

void SpriteAnimator::SetCurrentClip(const string& name)
{
	if (!m_clipsMap) {
		OutputDebugStringA("ERROR: SpriteAnimator::SetCurrentClip(name) - No animation clips map set.\n");
		return;
	}
	auto it = m_clipsMap->find(name);
	if (it == m_clipsMap->end()) {
		OutputDebugStringA(("ERROR: SpriteAnimator::SetCurrentClip(name) - Animation clip '" + name + "' not found.\n").c_str());
		// throw std::runtime_error("Animation clip not found: " + name);
		return;
	}
	m_currentClip = it->second; // 클립 복사
	m_elapsedTime = 0.0f;
	m_currentFrameIndex = 0;
	m_isLoop = m_currentClip.IsLoop();
}

void SpriteAnimator::Update(float deltaTime)
{
	if (!m_clipsMap || m_currentClip.GetFrames().empty()) {
		return;
	}

	m_elapsedTime += deltaTime;

	const vector<Frame>& frames = m_currentClip.GetFrames();

	if (m_elapsedTime >= frames[m_currentFrameIndex].duration)
	{
		m_elapsedTime -= frames[m_currentFrameIndex].duration; // 현재 프레임 시간을 빼서 다음 프레임의 시작점으로

		m_currentFrameIndex++;
		if (m_currentFrameIndex >= frames.size())
		{
			// 마지막 프레임에 도달
			if (m_isLoop)
			{
				m_currentFrameIndex = 0; // 반복
			}
			else
			{
				m_currentFrameIndex = static_cast<unsigned int>(frames.size() - 1); // 마지막 프레임에 멈춤
				m_elapsedTime = frames[m_currentFrameIndex].duration; // 애니메이션 종료 후 경과 시간을 마지막 프레임 지속 시간으로 고정
			}
		}
	}
}

const Frame& SpriteAnimator::GetCurrentFrame() const
{
	if (!m_clipsMap || m_currentClip.GetFrames().empty()) {
		
		static const Frame defaultFrame = {}; 
		return defaultFrame;
	}
	return m_currentClip.GetFrame(m_currentFrameIndex);

}