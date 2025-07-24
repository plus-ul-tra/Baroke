#pragma once
#include <filesystem>
#include "Singleton.h"

using namespace std;

class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;
	SoundManager() = default;
	~SoundManager() = default;

	std::unordered_map<string, FMOD::Sound*> m_sounds;
	FMOD::System* m_system = nullptr;

public:
	void Create();

	void LoadAll();
	void LoadSound(const filesystem::path& filePath);

	FMOD::System* GetSystem() const { return m_system; }
	FMOD::Sound* GetSound(const string& key) const;
	void PlaySoundOnce(const string& key);

	void Release();
};