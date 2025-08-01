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
using Board = array<array<shared_ptr<Stone>, SIZE_DEFAULT>, SIZE_DEFAULT>;

class BoardManager : public Singleton<BoardManager>
{
	friend class Singleton<BoardManager>;
	BoardManager() = default;
	~BoardManager() = default;

	Board m_board; // �ٵ���
	unordered_map<POINT, StoneType> m_stoneTypeMap; // ��ġ�� �� ���� �� // �缮 ������

	vector<pair<POINT, StoneAbility>> m_jokerPositions; // ��Ŀ �� ��ġ�� �ɷ� ����

	POINT m_selectedPosition = { -1, -1 }; // ��ǲ���� ���õ� �ٵ��� ��ġ
	StoneType m_stoneType = StoneType::Black;
	StoneAbility m_stoneAbility = StoneAbility::None;

	// Initialize ������
	int m_offX = 0, m_offY = 0, m_cell = 0, m_padding = 0, m_stoneOffset = 0;

	int CountLiberty // �缮 ������ �Լ�
	(
		int r, int c,
		std::vector<POINT>& group,
		std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT>& visited
	) const;
	void RemoveGroup(const std::vector<POINT>& group); // �缮 ������ �� �׷� ���� �Լ�

public:
	void Initialize(int offX, int offY, int _cell,int _stoneOffset, int padding = 0);
	void PlaceRandomStones(int amount); // ���� ���� �� �������� ���� ���� ���� �Լ�

	bool InputBasedGameLoop(POINT mousePos); // Ŭ������ �� ����
	bool InputBasedGameLoop(int row, int col); // �迭�� �������� �� ����

	void JokerAbilityUpdate(); // ��� ��Ŀ �ɷ� ����

	POINT MouseToBoardPosition(POINT mousePos) const; // ���콺 ��ǥ�� �ٵ��� ��ǥ�� ��ȯ
	POINT BoardToScreenPosition(POINT boardPos) const; // �ٵ��� ��ǥ�� ��ũ�� ��ǥ�� ��ȯ

	bool isValidPoint(POINT position) const { return position.x >= 0 && position.x < SIZE_DEFAULT && position.y >= 0 && position.y < SIZE_DEFAULT; }
	bool PlaceStone(POINT selectedPosition, StoneType stoneType, StoneAbility stoneAbility);
	void ResetStone();

	shared_ptr<Stone> GetStone(POINT);
	bool IsJokerStone(POINT position) const;

	Board GetBoard() const { return m_board; }
	int GetBoardSize() const { return SIZE_DEFAULT; }

	unordered_map<POINT, StoneType> GetStoneTypeMap() {	return  m_stoneTypeMap;	}
	int GetStoneTypeAmount(StoneType type) const;

	// ���� ���� �� ���� �Լ�
	void SetStoneType(StoneType type) { m_stoneType = type; }
	void SetStoneAbility(StoneAbility ability) { m_stoneAbility = ability; }
};