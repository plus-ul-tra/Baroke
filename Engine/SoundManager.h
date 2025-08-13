#pragma once
#include <filesystem>
#include "Singleton.h"

using namespace std;
using namespace FMOD;

class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;
	SoundManager() = default;
	~SoundManager() = default;

	unordered_map<string, Sound*> m_sounds;
	ChannelGroup* m_channelGroup = nullptr;
	System* m_system = nullptr;

public:
	void Initialize();

	void LoadAll();
	void LoadSound(const filesystem::path& filePath);

	Sound* GetSound(const string& key) const;
	ChannelGroup* GetChannelGroup() const { return m_channelGroup; }
	System* GetSystem() const { return m_system; }

	void PlaySoundOnce(const string& key);
	void ReleaseChannelGroup();

	void Release();
};