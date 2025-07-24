#pragma once
#include <fmod.hpp>
#include <filesystem>
#include <unordered_map>
#include "Singleton.h"

class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;
	SoundManager() = default;
	~SoundManager() = default;

	std::unordered_map<std::wstring, FMOD::Sound*> m_sounds;
	FMOD::System* m_system = nullptr;

public:

};