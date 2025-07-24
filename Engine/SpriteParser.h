#pragma once
#include "AnimationClip.h"
#include <filesystem>

using namespace std;
using AnimationClips = vector<pair<string, AnimationClip>>;

class SpriteParser
{
public:
	SpriteParser() = default;

	static AnimationClips Load(const filesystem::path& path);

	~SpriteParser() = default;
};