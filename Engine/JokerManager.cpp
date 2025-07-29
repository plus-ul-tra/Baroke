#include "pch.h"
#include "JokerManager.h"

void JokerManager::Initialize()
{
	{
		filesystem::path solutionRoot;
		try
		{
			solutionRoot = FindSolutionRoot();
		}
		catch (...)
		{
			solutionRoot = GetExecutableDir();
		}

		filesystem::path resourcePath = solutionRoot / L"Resource" / L"Json";
		auto jokers = JokerParser::LoadJson(resourcePath / L"StoneSheet.json");

		for (const auto& [name, info] : jokers)
		{
			m_jokerMap.emplace(name, info);
		}
	}
}

const JokerInfo& JokerManager::GetJokerInfo(const string& name) const
{
	{
		auto it = m_jokerMap.find(name);
		if (it == m_jokerMap.end()) throw runtime_error("해당 조커 정보를 찾을 수 없음");
		return it->second;
	}
}
