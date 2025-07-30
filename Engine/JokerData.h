#pragma once
#include <string>

struct JokerStoneData
{
	std::string id;
	std::string name;
	std::string description;
	std::string filename;
	std::string functionName;

	int type = 0;
	int price = 0;
	int activationCost = 0;
	int returnValue = 0;
	int duration = 0;
};
