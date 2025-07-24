#pragma once
#include <filesystem>
#include "Renderer.h"
#include "AnimationClip.h"

using namespace std;
using namespace Microsoft::WRL;
using AnimationClips = vector<pair<string, AnimationClip>>;

class SpriteManager
{
	unordered_map<wstring, ComPtr<ID2D1Bitmap1>> m_textures;
	unordered_map<wstring, AnimationClips> m_animationClips;
	Renderer* m_renderer = nullptr;

public:
	SpriteManager(Renderer* renderer) : m_renderer(renderer) {}

	void LoadAll();
	ID2D1Bitmap1* LoadTexture(const filesystem::path& filePath);
	void LoadAnimationClips(const filesystem::path& filePath);

	const ID2D1Bitmap1* GetTexture(const wstring& key) const;
	const AnimationClips& GetAnimationClips(const wstring& key) const;

	void Clear() { m_textures.clear(); m_animationClips.clear(); }

	~SpriteManager() = default;
};