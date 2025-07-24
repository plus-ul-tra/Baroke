#pragma once

using namespace std;
using namespace Microsoft::WRL;

struct Frame
{
	D2D1_RECT_U sourceRect;
	float duration;

	float Width() const { return static_cast<float>(sourceRect.right - sourceRect.left); }
	float Height() const { return static_cast<float>(sourceRect.bottom - sourceRect.top); }

	D2D1_RECT_F ToRectF() const
	{
		return D2D1::RectF(
			static_cast<float>(sourceRect.left),
			static_cast<float>(sourceRect.top),
			static_cast<float>(sourceRect.right),
			static_cast<float>(sourceRect.bottom));
	}
};

class AnimationClip
{
	vector<Frame> m_frames;
	float m_totalDuration = 0.0f;
	ComPtr<ID2D1Bitmap1> m_bitmap;

public:
	AnimationClip() = default;

	void SetBitmap(ComPtr<ID2D1Bitmap1> bitmap) { m_bitmap = move(bitmap); }
	void AddFrame(const Frame& frame) { m_frames.push_back(frame); m_totalDuration += frame.duration; }

	const vector<Frame>& GetFrames() const { return m_frames; }
	float GetTotalDuration() const { return m_totalDuration; }
	ID2D1Bitmap1* GetBitmap() const { return m_bitmap.Get(); }

	~AnimationClip() = default;
};

using AnimationClips = vector<pair<wstring, AnimationClip>>;
class SpriteAnimator
{
	const AnimationClips* m_clips = nullptr; // 애니메이션 클립 벡터
	AnimationClip m_clip; // 현재 애니메이션 클립
	unsigned int clipIndex = 0;

	float m_elapsedTime = 0.0f;
	bool m_isLoop = true;

public:
	SpriteAnimator() = default;

	bool IsValid() const { return m_clips != nullptr; }

	void SetClips(const AnimationClips* clips) { m_clips = clips; m_elapsedTime = 0.0f; } // 애니메이션 클립 벡터를 설정
	const AnimationClips* GetClips() const { return m_clips; } // 현재 애니메이션 클립 벡터를 반환

	void SetCurrentClip(unsigned int index = 0); // 인덱스로 현재 애니메이션 클립 벡터에서 애니메이션 클립을 설정
	void SetCurrentClip(const wstring& name); // 이름으로 현재 애니메이션 클립 벡터에서 애니메이션 클립을 설정
	const AnimationClip* GetCurrentClip() const; // 현재 애니메이션 클립을 반환

	void SetLoop(bool loop) { m_isLoop = loop; }
	const bool* GetLoop() const { return &m_isLoop; }

	void Update(float deltaTime);

	const Frame& GetCurrentFrame() const;

	float GetElapsedTime() const { return m_elapsedTime; }
	void SetElapsedTime(float elapsedTime) { m_elapsedTime = elapsedTime; }

	~SpriteAnimator() = default;
};