#pragma once

enum class StoneColor : uint8_t { None, Black, White , Special };
enum class StoneAbility : uint8_t { None };

struct StoneInfo
{
	StoneColor   color{ StoneColor::None };
	StoneAbility ability{ StoneAbility::None };
};


// ��ȭ ����
// ��Ʈȿ��
// �ڽ�Ʈ
// ����
