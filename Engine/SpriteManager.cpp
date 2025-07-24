#include "pch.h"
#include "SpriteManager.h"
#include "SpriteParser.h"

void SpriteManager::LoadAll()
{
	for (const auto& entry : filesystem::directory_iterator(L"C:\\Users\\User\\Desktop\\MiyabiEngine\\Resource\\Sprits"))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension() == L".png")
			{
				LoadTexture(entry.path());
			}
			if (entry.path().extension() == L".json")
			{
				LoadAnimationClips(entry.path());
			}
		}
	}
}

ID2D1Bitmap1* SpriteManager::LoadTexture(const filesystem::path& filePath)
{
	string key = filePath.stem().string();
	if (m_textures.find(key) == m_textures.end());
	{
		ComPtr<ID2D1Bitmap1> bitmap;
		m_renderer->CreateBitmapFromFile(filePath.c_str(), *m_textures[key].GetAddressOf());
		m_textures.emplace(key, bitmap);
	}

	return m_textures[key].Get();
}

void SpriteManager::LoadAnimationClips(const filesystem::path& filePath)
{
	filesystem::path texturePath = filePath;
	texturePath.replace_extension(L".png");
	ComPtr<ID2D1Bitmap1> bitmap = LoadTexture(texturePath);

	AnimationClips clips = SpriteParser::Load(filePath);

	for (auto& clip : clips) clip.second.SetBitmap(bitmap);

	m_animationClips.emplace(filePath.stem().string(), move(clips));
}

const ID2D1Bitmap1* SpriteManager::GetTexture(const string& key) const
{
	auto it = m_textures.find(key);
	if (it == m_textures.end()) throw runtime_error("해당 이미지를 찾을 수 없음");

	return it->second.Get();
}

const AnimationClips& SpriteManager::GetAnimationClips(const string& key) const
{
	auto it = m_animationClips.find(key);
	if (it == m_animationClips.end()) throw runtime_error("해당 애니메이션 클립을 찾을 수 없음");

	return it->second;
}

void SpriteManager::Release()
{
	for (auto& texture : m_textures)
	{
		if (texture.second)
		{
			texture.second.Reset();
		}
	}
	m_textures.clear();
	for (auto& clips : m_animationClips)
	{
		clips.second.clear();
	}
	m_animationClips.clear();
}