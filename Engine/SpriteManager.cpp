#include "pch.h"
#include "SpriteManager.h"
#include "SpriteParser.h"

void SpriteManager::LoadAll()
{
	for (const auto& entry : filesystem::directory_iterator(L"Resources"))
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
	auto key = filePath.stem().wstring();
	if (m_textures.find(key) == m_textures.end());
	{
		ComPtr<ID2D1Bitmap1> bitmap;
		m_renderer->CreateBitmapFromFile(filePath.c_str(), *m_textures[key].GetAddressOf());
		m_textures[key] = move(bitmap);
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

	m_animationClips[filePath.stem().wstring()] = move(clips);
}

const ID2D1Bitmap1* SpriteManager::GetTexture(const wstring& key) const
{
	auto it = m_textures.find(key);
	if (it == m_textures.end()) throw runtime_error("해당 이미지를 찾을 수 없음");

	return it->second.Get();
}

const AnimationClips& SpriteManager::GetAnimationClips(const wstring& key) const
{
	auto it = m_animationClips.find(key);
	if (it == m_animationClips.end()) throw runtime_error("해당 애니메이션 클립을 찾을 수 없음");

	return it->second;
}