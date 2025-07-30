#pragma once
#include "AnimationClip.h"
#include <filesystem>

using namespace std;
using AnimationClipsMap = map<string, AnimationClip>;

class SpriteParser
{
public:
	static AnimationClipsMap Load(const filesystem::path& path);
};