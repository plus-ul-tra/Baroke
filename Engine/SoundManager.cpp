#include "pch.h"
#include "SoundManager.h"
#include "FileDirectory.h"

void SoundManager::Initialize()
{
	System_Create(&m_system);
	m_system->init(512, FMOD_INIT_NORMAL, nullptr);
	m_system->createChannelGroup("Master", &m_channelGroup);
	m_system->createChannelGroup("SFX", &m_sfxChannelGroup);
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

	for (const auto& entry : filesystem::recursive_directory_iterator(resourcePath))

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
		Sound* sound = nullptr;
		m_system->createSound(filePath.string().c_str(), FMOD_DEFAULT, nullptr, &sound);
		m_sounds.emplace(key, sound);
	}
}

Sound* SoundManager::GetSound(const string& key) const
{
	auto it = m_sounds.find(key);
	if (it == m_sounds.end()) throw runtime_error("해당 사운드를 찾을 수 없음");

	return it->second;
}

void SoundManager::PlaySoundOnce(const string& key)
{
	ReleaseSfxChannelGroup();

	Sound* sound = GetSound(key);
	sound->setMode(FMOD_LOOP_OFF); // 단일 재생

	Channel* newChannel = nullptr;

	if (sound) m_system->playSound(sound, m_sfxChannelGroup, false, &newChannel);
	else throw runtime_error("해당 사운드를 찾을 수 없음");
	newChannel->setVolume(SFX_VOLUME); // SFX 볼륨 설정
}

void SoundManager::ReleaseChannelGroup()
{
	if (m_channelGroup)
	{
		int channelCount = 0;
		m_channelGroup->getNumChannels(&channelCount);
		for (int i = 0; i < channelCount; ++i)
		{
			Channel* channel = nullptr;
			m_channelGroup->getChannel(i, &channel);
			if (channel) channel->stop();
		}

		m_channelGroup->release();
		m_channelGroup = nullptr;
	}
	m_system->createChannelGroup("Master", &m_channelGroup);
}

void SoundManager::ReleaseSfxChannelGroup()
{
	if (m_sfxChannelGroup)
	{
		int channelCount = 0;
		m_sfxChannelGroup->getNumChannels(&channelCount);
		for (int i = 0; i < channelCount; ++i)
		{
			Channel* channel = nullptr;
			m_sfxChannelGroup->getChannel(i, &channel);
			if (channel) channel->stop();
		}

		m_sfxChannelGroup->release();
		m_sfxChannelGroup = nullptr;
	}
	m_system->createChannelGroup("SFX", &m_sfxChannelGroup);
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
