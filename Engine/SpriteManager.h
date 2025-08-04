#pragma once
#include <filesystem>
#include <string>
#include <map>
#include <vector>
#include <wrl/client.h>
#include <wincodec.h>

#include <d3d11.h>

#include "Singleton.h"
#include "Renderer.h"
#include "AnimationClip.h"

using namespace std;
using namespace Microsoft::WRL;

using AnimationClipsMap = std::map<std::string, AnimationClip>;


class SpriteManager : public Singleton<SpriteManager>
{
private:
	friend class Singleton<SpriteManager>;

	SpriteManager() = default;
	~SpriteManager() { Release(); }

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	ComPtr<IWICImagingFactory> m_pWICFactory;

	unordered_map<string, ComPtr<ID3D11ShaderResourceView>> m_textures;
	unordered_map<string, AnimationClipsMap> m_animationClips;
	//Renderer* m_renderer = nullptr;

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContextr);
	void LoadAll();

	// png 읽어서 -> SRV로 변환
	ComPtr<ID3D11ShaderResourceView> LoadTextureSRV(const filesystem::path& filePath);

	void LoadAnimationClips(const filesystem::path& filePath);
	ID3D11Device* GetDevice() { return m_pDevice; }
	const ComPtr<ID3D11ShaderResourceView> GetTextureSRV(const string& key) const;
	const AnimationClipsMap& GetAnimationClips(const string& key) const;
	bool IsAnimatedSprite(const string& key) const;
	void Release();
};