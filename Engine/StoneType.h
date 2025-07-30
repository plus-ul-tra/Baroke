#pragma once

enum class StoneColor : uint8_t { None, Black, White , Special };

enum class StoneAbility : uint8_t { None, ability1, ability2, ability3 };


struct StoneInfo
{
	StoneColor   color{ StoneColor::None };
	StoneAbility ability{ StoneAbility::None };
};


// 진화 여부
// 세트효과
// 코스트
// 리턴
