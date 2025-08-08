#pragma once
#include <string>
#include <map>

using namespace std;
using namespace Microsoft::WRL;

struct Frame
{
	RECT sourceRect;
	float duration;

	//일단 필요없음
	//float Width() const { return static_cast<float>(sourceRect.right - sourceRect.left); }
	//float Height() const { return static_cast<float>(sourceRect.bottom - sourceRect.top); }

	/*D2D1_RECT_F ToRectF() const
	{
		return D2D1::RectF(
			static_cast<float>(sourceRect.left),
			static_cast<float>(sourceRect.top),
			static_cast<float>(sourceRect.right),
			static_cast<float>(sourceRect.bottom));
	}*/
};

// 
class AnimationClip
{
	string m_name; //spriteAnimator에서 특정 clip 찾을 때 사용
	vector<Frame> m_frames;
	float m_totalDuration = 0.0f;
	bool m_isLoop = true;
	ComPtr<ID3D11ShaderResourceView> m_textureSRV;

public:
	AnimationClip() = default;

	//void SetBitmap(ComPtr<ID2D1Bitmap1> bitmap) { m_bitmap = move(bitmap); }
	void SetTextureSRV(ComPtr<ID3D11ShaderResourceView> srv) { m_textureSRV = srv; }
	ID3D11ShaderResourceView* GetTextureSRV() const { return m_textureSRV.Get(); }

	void AddFrame(const Frame& frame) { m_frames.push_back(frame); m_totalDuration += frame.duration; }

	const string& GetName() const { return m_name; }
	void SetName(const string& name) { m_name = name; }

	const vector<Frame>& GetFrames() const { return m_frames; }
	float GetTotalDuration() const { return m_totalDuration; }

	bool IsLoop() const { return m_isLoop; }
	void SetLoop(bool loop) { m_isLoop = loop; }

	const Frame& GetFrame(unsigned int index) const
	{
		if (index >= m_frames.size()) {

			return m_frames[0];
		}
		return m_frames[index];
	}

	~AnimationClip() = default;
};

//using AnimationClips = vector<pair<string, AnimationClip>>;
using AnimationClipsMap = map<string, AnimationClip>; // 탐색을 위해 Map으로 변경

class SpriteAnimator
{
private:
	const AnimationClipsMap* m_clipsMap = nullptr;

	// 현재 애니메이션 클립
	AnimationClip m_currentClip;
	unsigned int m_currentFrameIndex = 0;

	float m_elapsedTime = 0.0f;
	bool m_isLoop = false;

public:
	SpriteAnimator() = default;

	bool IsValid() const { return m_clipsMap != nullptr; }

	void SetClips(const AnimationClipsMap* clipsMap) {
		m_clipsMap = clipsMap;
		m_elapsedTime = 0.0f;
		m_currentFrameIndex = 0; // 클립 설정 시 초기화
		if (m_clipsMap && !m_clipsMap->empty()) {
			// 기본 클립을 첫 번째 클립으로 설정 (또는 명시적으로 SetCurrentClip 호출)
			SetCurrentClip(m_clipsMap->begin()->first);
		}
	}

	const AnimationClipsMap* GetClipsMap() const { return m_clipsMap; } // 현재 애니메이션 클립 맵을 반환

	void SetCurrentClip(unsigned int index); // 인덱스로 현재 애니메이션 클립을 설정
	void SetCurrentClip(const string& name); // 이름으로 현재 애니메이션 클립을 설정

	const AnimationClip* GetCurrentClip() const { return &m_currentClip; } // 현재 애니메이션 클립을 반환

	void SetLoop(bool loop) { m_isLoop = loop; }
	const bool* GetLoop() const { return &m_isLoop; }

	bool Update(float deltaTime);

	const Frame& GetCurrentFrame() const;

	float GetElapsedTime() const { return m_elapsedTime; }
	void SetElapsedTime(float elapsedTime) { m_elapsedTime = elapsedTime; }

	~SpriteAnimator() = default;
};