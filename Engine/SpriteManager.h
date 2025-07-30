#pragma once
#include <filesystem>
#include "Singleton.h"
#include "Renderer.h"
#include "AnimationClip.h"

using namespace std;
using namespace Microsoft::WRL;
using AnimationClips = vector<pair<string, AnimationClip>>;

class SpriteManager : public Singleton<SpriteManager>
{
	friend class Singleton<SpriteManager>;
	SpriteManager() = default;
	~SpriteManager() = default;

	unordered_map<string, ComPtr<ID2D1Bitmap1>> m_textures;
	unordered_map<string, AnimationClips> m_animationClips;
	shared_ptr<Renderer> m_renderer = nullptr;

public:
	void Initialize(shared_ptr<Renderer> renderer) { m_renderer = renderer; LoadAll(); }

	void LoadAll();
	ComPtr<ID2D1Bitmap1> LoadTexture(const filesystem::path& filePath);
	void LoadAnimationClips(const filesystem::path& filePath);

	const ComPtr<ID2D1Bitmap1> GetTexture(const string& key) const;
	const AnimationClips& GetAnimationClips(const string& key) const;

	void Release();
};