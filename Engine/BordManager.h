#pragma once
#include "Engine.h"
#include "Stone.h"
#include "Singleton.h"

enum StoneType // �� ����
{
	White,
	Black,
	Joker
};

enum StoneAbility // �ɷ� Ȥ�� �̸�
{
	None, // �浹, �鵹

	JokerAbility1,
	JokerAbility2,
	JokerAbility3
};

unordered_map<StoneAbility, function<void()>> g_abilityFunctions; // ��򰡿��� �ʱ�ȭ�Ǿ�� ��

constexpr int SIZE_DEFAULT = 15;
class BordManager : public Singleton<BordManager>
{
	friend class Singleton<BordManager>;
	BordManager() = default;
	~BordManager() = default;

	array<array<shared_ptr<Stone>, SIZE_DEFAULT>, SIZE_DEFAULT> m_board; // �ٵ���

	struct Position { int row; int col; };
	unordered_map<Position, StoneType> m_stoneTypeMap; // ��ġ�� �� ���� ��
	vector<pair<Position, StoneAbility>> m_jokerPositions; // ��Ŀ �� ��ġ�� �ɷ�

	StoneType m_stoneType = StoneType::White;
	StoneAbility m_stoneAbility = StoneAbility::None;

	void PlaceStone(int row, int col, StoneType stoneType, StoneAbility stoneAbility);
	shared_ptr<Stone> GetStone(int row, int col);
};