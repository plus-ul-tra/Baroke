#pragma once
#include "AnimationClip.h"

using namespace std;
using AnimationClips = vector<pair<wstring, AnimationClip>>;

class SpriteParser
{
public:
	SpriteParser() = default;

	static AnimationClips Load(const filesystem::path& path);

	~SpriteParser() = default;
};