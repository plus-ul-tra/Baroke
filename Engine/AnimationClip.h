#pragma once
#include <string>
#include <map>

using namespace std;
using namespace Microsoft::WRL;

struct Frame
{
	RECT sourceRect;
	float duration;

	//�ϴ� �ʿ����
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
	string m_name; //spriteAnimator���� Ư�� clip ã�� �� ���
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
using AnimationClipsMap = map<string, AnimationClip>; // Ž���� ���� Map���� ����

class SpriteAnimator
{
private:
	const AnimationClipsMap* m_clipsMap = nullptr;

	// ���� �ִϸ��̼� Ŭ��
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
		m_currentFrameIndex = 0; // Ŭ�� ���� �� �ʱ�ȭ
		if (m_clipsMap && !m_clipsMap->empty()) {
			// �⺻ Ŭ���� ù ��° Ŭ������ ���� (�Ǵ� ��������� SetCurrentClip ȣ��)
			SetCurrentClip(m_clipsMap->begin()->first);
		}
	}

	const AnimationClipsMap* GetClipsMap() const { return m_clipsMap; } // ���� �ִϸ��̼� Ŭ�� ���� ��ȯ

	void SetCurrentClip(unsigned int index); // �ε����� ���� �ִϸ��̼� Ŭ���� ����
	void SetCurrentClip(const string& name); // �̸����� ���� �ִϸ��̼� Ŭ���� ����

	const AnimationClip* GetCurrentClip() const { return &m_currentClip; } // ���� �ִϸ��̼� Ŭ���� ��ȯ

	void SetLoop(bool loop) { m_isLoop = loop; }
	const bool* GetLoop() const { return &m_isLoop; }

	bool Update(float deltaTime);

	const Frame& GetCurrentFrame() const;

	float GetElapsedTime() const { return m_elapsedTime; }
	void SetElapsedTime(float elapsedTime) { m_elapsedTime = elapsedTime; }

	~SpriteAnimator() = default;
};