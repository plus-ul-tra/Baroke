#pragma once

enum class StoneColor : uint8_t { None, Black, White , Special };

enum class StoneAbility : uint8_t { None, ability1, ability2, ability3 };


struct StoneInfo
{
	StoneColor   color{ StoneColor::None };
	StoneAbility ability{ StoneAbility::None };
};


// ��ȭ ����
// ��Ʈȿ��
// �ڽ�Ʈ
// ����
