#pragma once
#include <filesystem>

using namespace std;

enum class StoneType
{
	Passive,
	Active,
	Trigger
};
struct Stone
{
	string description;
	string fileName;
	string functionName;
	StoneType type;
	int price;
	int activationCost;
	int returnValue;
	int duration;
};

class StoneParser
{
	public:
		static vector<pair<string, Stone>> LoadJson(const filesystem::path& path);
		static vector<pair<string, Stone>> LoadCsv(const filesystem::path& path);
};

