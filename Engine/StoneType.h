#pragma once

enum class StoneColor : uint8_t { None, Black, White };
enum class StoneAbility : uint8_t { None };

struct StoneInfo
{
	StoneColor   color{ StoneColor::None };
	StoneAbility ability{ StoneAbility::None };
};


