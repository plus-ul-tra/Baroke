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

	// ��� ���� �������� ������ ���� (JSON�� "frames" �迭�� ��ġ)
	// AnimationClip.h���� RECT�� ����ϵ��� Frame ����ü ���ǰ� ����Ǿ��ٰ� �����մϴ�.
	vector<Frame> allFrames;
	allFrames.reserve(doc["frames"].size());

	for (const auto& frame : doc["frames"])
	{
		auto& rect = frame["frame"];

		allFrames.push_back
		(
			Frame
			{
				// D2D1_RECT_U ��� RECT�� ���
				RECT
				{
					rect["x"].get<LONG>(), // UINT ��� LONG ��� (RECT�� ����� LONG)
					rect["y"].get<LONG>(),
					static_cast<LONG>(rect["x"].get<LONG>() + rect["w"].get<LONG>()),
					static_cast<LONG>(rect["y"].get<LONG>() + rect["h"].get<LONG>())
				},
			frame["duration"].get<float>()
			}
		);
	}

	// AnimationClipsMap (std::map<string, AnimationClip>)�� ��ȯ�ϵ��� ����
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
		clip.SetName(name); // AnimationClip�� �̸��� �����ϴ� �Լ� �߰�
		clip.SetLoop(tag.contains("loop") ? tag["loop"].get<bool>() : false); // Aseprite JSON�� loop ������ �ִٸ� Ȱ��

		for (int i = from; i <= to; ++i)
		{
			// allFrames���� ������ Frame�� AnimationClip�� �߰�
			clip.AddFrame(allFrames[i]);
		}

		// std::map�� ����: �̸�(string)�� Ű��, AnimationClip ��ü�� ������
		clipsMap.emplace(move(name), move(clip)); // move�� ����Ͽ� ȿ�������� ����
	}

	return clipsMap;
}