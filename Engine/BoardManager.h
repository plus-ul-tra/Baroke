#pragma once
#include "Singleton.h"
#include "Stone.h"

namespace std
{
	template <>
	struct hash<POINT>
	{
		std::size_t operator()(const POINT& pt) const noexcept
		{
			return std::hash<LONG>()(pt.x) ^ (std::hash<LONG>()(pt.y) << 1);
		}
	};
}
inline bool operator==(const POINT& lhs, const POINT& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr int SIZE_DEFAULT = 15;

class BoardManager : public Singleton<BoardManager>
{
	friend class Singleton<BoardManager>;
	BoardManager() = default;
	~BoardManager() = default;

	array<array<shared_ptr<Stone>, SIZE_DEFAULT>, SIZE_DEFAULT> m_board; // �ٵ���
	unordered_map<POINT, StoneType> m_stoneTypeMap; // ��ġ�� �� ���� �� // �缮 ������

	vector<pair<POINT, StoneAbility>> m_jokerPositions; // ��Ŀ �� ��ġ�� �ɷ� ����

	POINT m_selectedPosition = { -1, -1 }; // ��ǲ���� ���õ� �ٵ��� ��ġ
	StoneType m_stoneType = StoneType::White;
	StoneAbility m_stoneAbility = StoneAbility::None;

public:
	void Initialize();
	void PlaceRandomStones(int amount); // ���� ���� �� �������� ���� ���� ���� �Լ� // ���� �̱���

	void InputBasedGameLoop(POINT mousePos); // Ŭ������ �� ����
	void JokerAbilityUpdate(); // ��Ŀ �ɷ� ����

	// ���콺 ��ġ�� �ٵ��� ��ġ ��ȯ �Լ� ���� �̱���
	POINT MouseToBoardPosition(POINT mousePos) const { return { mousePos.x / 50, mousePos.y / 50 }; }
	POINT BoardToScreenPosition(POINT boardPos) const { return { boardPos.x * 50, boardPos.y * 50 }; }

	bool PlaceStone(POINT selectedPosition, StoneType stoneType, StoneAbility stoneAbility);
	shared_ptr<Stone> GetStone(POINT);
};