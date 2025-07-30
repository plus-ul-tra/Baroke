#pragma once

enum class ItemType { None, Passive, Active  };

enum class ItemAbility { None };


struct ItemInfo
{
	ItemType   type{ ItemType::None };
	ItemAbility ability{ ItemAbility::None };
};
