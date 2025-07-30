#pragma once

enum class StoneType : uint8_t { None, Black, White , Joker };

enum class StoneAbility : uint8_t { None, ability1, ability2, ability3 };

enum class StoneSetOption : uint8_t { None };


struct StoneInfo
{
	StoneInfo() {}
	StoneInfo(StoneType color, StoneAbility ability)
		: color(color), ability(ability) {}
	std::string		name		= "default";
	StoneType		color		{ StoneType::None };
	StoneAbility	ability		{ StoneAbility::None };
	StoneSetOption	setoption	{ StoneSetOption::None };
	bool			is_upgrade	 = false;
	int				cost		 = 0;
	int				returnCost	 = 0;
};

