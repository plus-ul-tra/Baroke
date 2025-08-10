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
	System* m_system = nullptr;

public:
	void Initialize();

	void LoadAll();
	void LoadSound(const filesystem::path& filePath);

	System* GetSystem() const { return m_system; }
	Sound* GetSound(const string& key) const;

	void PlaySoundOnce(const string& key);

	void Release();
};