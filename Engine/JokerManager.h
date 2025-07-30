#pragma once
//#include "JokerParser.h"
//#include "FileDirectory.h"

//
//class JokerManager : public Singleton<JokerManager>
//{
//	friend class Singleton<JokerManager>;
//	JokerManager() = default;
//	~JokerManager() = default;
//
//	unordered_map<string, JokerInfo> m_jokerMap; // string = stonename
//
//public:
//	void Initialize();
//
//	const JokerInfo& GetJokerInfo(const string& name) const;
//};

#pragma once
#include "Singleton.h"
#include "JokerData.h" // JokerStoneData 구조체 정의 파일
#include "StoneType.h"

class JokerManager : public Singleton<JokerManager>
{
public:

	void Initialize(); 
	void RegisterJoker(const JokerStoneData& data);

	const JokerStoneData& GetJokerInfo(const std::string& name) const;

	bool HasJokerInfo(const std::string& name) const;

	StoneInfo CreateStoneInfoFromJoker(const std::string& name) const;

private:
	friend class Singleton<JokerManager>;
	JokerManager() = default;
	~JokerManager() = default;

	std::unordered_map<std::string, JokerStoneData> m_jokerMap;

};

