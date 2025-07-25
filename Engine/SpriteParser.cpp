#include "pch.h"
#include "SpriteParser.h"
#include <fstream>
#include "json.hpp"

AnimationClips SpriteParser::Load(const filesystem::path& path)
{
	ifstream in{ path };
	if (!in || !in.is_open()) throw runtime_error(path.string() + "열기 실패");

	nlohmann::json doc = nlohmann::json::parse(in);
	vector<Frame> frames;
	frames.reserve(doc["frames"].size());

	for (const auto& frame : doc["frames"])
	{
		auto& rect = frame["frame"];

		frames.push_back
		(
			Frame
			{
				D2D1_RECT_U
				{
					rect["x"].get<UINT>(),
					rect["y"].get<UINT>(),
					static_cast<UINT>(rect["x"].get<UINT>() + rect["w"].get<UINT>()),
					static_cast<UINT>(rect["y"].get<UINT>() + rect["h"].get<UINT>())
				},
			frame["duration"].get<float>()
			}
		);
	}

	AnimationClips clips;
	for (const auto& tag : doc["meta"]["frameTags"])
	{
		string name = tag["name"];
		int from = tag["from"];
		int to = tag["to"];
		if (from > to) throw runtime_error("잘못된 프레임 범위");

		AnimationClip clip;
		for (int i = from; i <= to; ++i) clip.AddFrame(frames[i]);
		clips.emplace_back(move(name), move(clip));
	}

	return clips;
}