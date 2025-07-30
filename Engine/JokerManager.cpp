#include "pch.h"
#include "JokerManager.h"

//void JokerManager::Initialize()
//{
//	{
//		filesystem::path solutionRoot;
//		try
//		{
//			solutionRoot = FindSolutionRoot();
//		}
//		catch (...)
//		{
//			solutionRoot = GetExecutableDir();
//		}
//
//		filesystem::path resourcePath = solutionRoot / L"Resource" / L"Json";
//		auto jokers = JokerParser::LoadJson(resourcePath / L"StoneSheet.json");
//
//		for (const auto& [name, info] : jokers)
//		{
//			m_jokerMap.emplace(name, info);
//		}
//	}
//}
//
//const JokerInfo& JokerManager::GetJokerInfo(const string& name) const
//{
//	{
//		auto it = m_jokerMap.find(name);
//		if (it == m_jokerMap.end()) throw runtime_error("해당 조커 정보를 찾을 수 없음");
//		return it->second;
//	}
//}



void JokerManager::Initialize()
{
	JokerStoneData stone1;
	stone1.id = "1";
	stone1.name = "JokerStone1";
	stone1.description = "Description of JokerStone1";
	stone1.filename = "JokerStone1.png";
	stone1.functionName = "JokerStone1Function";
	stone1.type = 0;
	stone1.price = 15;
	stone1.activationCost = 2;
	stone1.returnValue = 5;
	stone1.duration = 3;
	RegisterJoker(stone1);

	JokerStoneData stone2;
	stone2.id = "2";
	stone2.name = "JokerStone2";
	stone2.description = "Description of JokerStone2";
	stone2.filename = "JokerStone2.png";
	stone2.functionName = "JokerStone2Function";
	stone2.type = 1;
	stone2.price = 25;
	stone2.activationCost = 3;
	stone2.returnValue = 5;
	stone2.duration = 3;
	RegisterJoker(stone2);
}

void JokerManager::RegisterJoker(const JokerStoneData& data)
{
	m_jokerMap[data.name] = data;
}

const JokerStoneData& JokerManager::GetJokerInfo(const std::string& name) const
{
	auto it = m_jokerMap.find(name);
	if (it == m_jokerMap.end())
	{
		std::cerr << "JokerManager: '" << name << "' not found!\n";
		throw std::runtime_error("Joker not found: " + name);
	}
	return it->second;
}

bool JokerManager::HasJokerInfo(const std::string& name) const
{
	return m_jokerMap.find(name) != m_jokerMap.end();
}


StoneInfo JokerManager::CreateStoneInfoFromJoker(const std::string& name) const
{
	const auto& data = GetJokerInfo(name); // 없으면 예외 터짐

	StoneInfo info;
	info.color = StoneType::Joker;  
	info.ability = static_cast<StoneAbility>(data.type);
	info.cost = data.activationCost;
	info.returnCost = data.returnValue;
	info.is_upgrade = false;
	info.setoption = StoneSetOption::None; // 필요 시

	return info;
}
