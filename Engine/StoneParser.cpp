#include "pch.h"
#include "StoneParser.h"
#include <fstream>
#include "json.hpp"

vector<pair<string, Stone>> StoneParser::LoadJson(const filesystem::path& path)
{
	ifstream in{ path };
	if (!in || !in.is_open()) throw runtime_error(path.string() + "열기 실패");

	nlohmann::json doc = nlohmann::json::parse(in);
	vector<pair<string, Stone>> Stones;
	Stones.reserve(doc["stones"].size());

	for (const auto& stone : doc["stones"])
	{
		Stone s;
		s.description = stone["description"].get<string>();
		s.fileName = stone["fileName"].get<string>();
		s.functionName = stone["functionName"].get<string>();
		s.type = stone["type"].get<StoneType>();
		s.price = stone["price"].get<int>();
		s.activationCost = stone["activationCost"].get<int>();
		s.returnValue = stone["returnValue"].get<int>();
		s.duration = stone["duration"].get<int>();

		Stones.emplace_back(stone["name"].get<string>(), move(s));
	}
	return Stones;
}

vector<pair<string, Stone>> StoneParser::LoadCsv(const filesystem::path& path)
{
	ifstream in{ path };
	if (!in || !in.is_open()) throw runtime_error(path.string() + "열기 실패");
	vector<pair<string, Stone>> Stones;
	string line;
	while (getline(in, line))
	{
		istringstream ss(line);
		string name, description, fileName, functionName;
		int type, price, activationCost, returnValue, duration;
		getline(ss, name, ',');
		getline(ss, description, ',');
		getline(ss, fileName, ',');
		getline(ss, functionName, ',');
		ss >> type;
		ss.ignore(1); // 콤마 무시
		ss >> price;
		ss.ignore(1);
		ss >> activationCost;
		ss.ignore(1);
		ss >> returnValue;
		ss.ignore(1);
		ss >> duration;

		Stone s
		{
			description, fileName, functionName,
			static_cast<StoneType>(type),
			price, activationCost, returnValue, duration
		};

		Stones.emplace_back(name, s);
	}
	return Stones;
}