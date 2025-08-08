#include "pch.h"
#include "SpriteParser.h"
#include <fstream>
#include "json.hpp"

AnimationClipsMap SpriteParser::Load(const filesystem::path& path)
{
	ifstream in{ path };
	if (!in || !in.is_open())
	{
		throw runtime_error("SpriteParser: Failed to open JSON file: " + path.string());
	}

	nlohmann::json doc = nlohmann::json::parse(in);

	// 모든 개별 프레임을 저장할 벡터 (JSON의 "frames" 배열과 일치)
	// AnimationClip.h에서 RECT를 사용하도록 Frame 구조체 정의가 변경되었다고 가정합니다.
	vector<Frame> allFrames;
	allFrames.reserve(doc["frames"].size());

	for (const auto& frame : doc["frames"])
	{
		auto& rect = frame["frame"];

		allFrames.push_back
		(
			Frame
			{
				// D2D1_RECT_U 대신 RECT를 사용
				RECT
				{
					rect["x"].get<LONG>(), // UINT 대신 LONG 사용 (RECT의 멤버는 LONG)
					rect["y"].get<LONG>(),
					static_cast<LONG>(rect["x"].get<LONG>() + rect["w"].get<LONG>()),
					static_cast<LONG>(rect["y"].get<LONG>() + rect["h"].get<LONG>())
				},
			frame["duration"].get<float>()
			}
		);
	}

	// AnimationClipsMap (std::map<string, AnimationClip>)을 반환하도록 변경
	AnimationClipsMap clipsMap;

	if (!doc["meta"].contains("frameTags")) {

		return clipsMap;
	}


	for (const auto& tag : doc["meta"]["frameTags"])
	{
		string name = tag["name"];
		int from = tag["from"];
		int to = tag["to"];

		if (from < 0 || to < 0 || from >= allFrames.size() || to >= allFrames.size() || from > to)
		{
			throw runtime_error("SpriteParser: Invalid frame range in JSON for clip '" + name + "': " + path.string());
		}

		AnimationClip clip;
		clip.SetName(name); // AnimationClip에 이름을 설정하는 함수 추가
		clip.SetLoop(tag.contains("loop") ? tag["loop"].get<bool>() : false); // Aseprite JSON에 loop 정보가 있다면 활용

		for (int i = from; i <= to; ++i)
		{
			// allFrames에서 가져온 Frame을 AnimationClip에 추가
			clip.AddFrame(allFrames[i]);
		}

		// std::map에 삽입: 이름(string)을 키로, AnimationClip 객체를 값으로
		clipsMap.emplace(move(name), move(clip)); // move를 사용하여 효율적으로 삽입
	}

	return clipsMap;
}