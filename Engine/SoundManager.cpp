#include "pch.h"
#include "SoundManager.h"
#include "FileDirectory.h"

void SoundManager::Initialize()
{
	FMOD::System_Create(&m_system);
	m_system->init(512, FMOD_INIT_NORMAL, nullptr);
	LoadAll();
}

void SoundManager::LoadAll()
{
	filesystem::path solutionRoot;
	try
	{
		solutionRoot = FindSolutionRoot();
	}
	catch (...)
	{
		solutionRoot = GetExecutableDir();
	}
	filesystem::path resourcePath = solutionRoot/L"Resource"/L"Sounds";

	for (const auto& entry : std::filesystem::directory_iterator(resourcePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension() == L".wav" || entry.path().extension() == L".mp3" || entry.path().extension() == L".ogg")
			{
				LoadSound(entry.path());
			}
		}
	}
}

void SoundManager::LoadSound(const filesystem::path& filePath)
{
	string key = filePath.filename().string();
	if (m_sounds.find(key) == m_sounds.end())
	{
		FMOD::Sound* sound = nullptr;
		m_system->createSound(filePath.string().c_str(), FMOD_DEFAULT, nullptr, &sound);
		m_sounds.emplace(key, sound);
	}
}

FMOD::Sound* SoundManager::GetSound(const string& key) const
{
	auto it = m_sounds.find(key);
	if (it == m_sounds.end()) throw runtime_error("해당 사운드를 찾을 수 없음");

	return it->second;
}

void SoundManager::PlaySoundOnce(const string& key)
{
	FMOD::Sound* sound = GetSound(key);
	if (sound) m_system->playSound(sound, nullptr, false, nullptr);
	else throw runtime_error("해당 사운드를 찾을 수 없음");
}

void SoundManager::Release()
{
	for (auto& sound : m_sounds)
	{
		if (sound.second)
		{
			sound.second->release();
			sound.second = nullptr;
		}
	}
	m_sounds.clear();
	if (m_system)
	{
		m_system->release();
		m_system = nullptr;
	}
}
