#include "pch.h"
//#include "JokerParser.h"
//#include <fstream>
//#include "json.hpp"
//
//vector<pair<string, JokerInfo>> JokerParser::LoadJson(const filesystem::path& path)
//{
//	ifstream in{ path };
//	if (!in || !in.is_open()) throw runtime_error(path.string() + "열기 실패");
//
//	nlohmann::json doc = nlohmann::json::parse(in);
//	vector<pair<string, JokerInfo>> Jokers;
//	Jokers.reserve(doc["stones"].size());
//
//	for (const auto& stone : doc["stones"])
//	{
//		JokerInfo s;
//		s.description = stone["description"].get<string>();
//		s.fileName = stone["filename"].get<string>();
//		s.functionName = stone["function"].get<string>();
//		s.type = stone["type"].get<JokerType>();
//		s.price = stone["price"].get<int>();
//		s.activationCost = stone["activationCost"].get<int>();
//		s.returnValue = stone["returnValue"].get<int>();
//		s.duration = stone["duration"].get<int>();
//
//		Jokers.emplace_back(stone["stonename"].get<string>(), move(s));
//	}
//	return Jokers;
//}
//
//vector<pair<string, JokerInfo>> JokerParser::LoadCsv(const filesystem::path& path)
//{
//	ifstream in{ path };
//	if (!in || !in.is_open()) throw runtime_error(path.string() + "열기 실패");
//	vector<pair<string, JokerInfo>> Jokers;
//	string line;
//	while (getline(in, line))
//	{
//		istringstream ss(line);
//		string name, description, fileName, functionName;
//		int type, price, activationCost, returnValue, duration;
//		getline(ss, name, ',');
//		getline(ss, description, ',');
//		getline(ss, fileName, ',');
//		getline(ss, functionName, ',');
//		ss >> type;
//		ss.ignore(1); // 콤마 무시
//		ss >> price;
//		ss.ignore(1);
//		ss >> activationCost;
//		ss.ignore(1);
//		ss >> returnValue;
//		ss.ignore(1);
//		ss >> duration;
//
//		JokerInfo s
//		{
//			description, fileName, functionName,
//			static_cast<JokerType>(type),
//			price, activationCost, returnValue, duration
//		};
//
//		Jokers.emplace_back(name, s);
//	}
//	return Jokers;
//}