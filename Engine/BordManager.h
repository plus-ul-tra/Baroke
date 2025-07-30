#pragma once
#include "Engine.h"
#include "Stone.h"
#include "Singleton.h"

enum StoneType // 돌 종류
{
	White,
	Black,
	Joker
};

enum StoneAbility // 능력 혹은 이름
{
	None, // 흑돌, 백돌

	JokerAbility1,
	JokerAbility2,
	JokerAbility3
};

unordered_map<StoneAbility, function<void()>> g_abilityFunctions; // 어딘가에서 초기화되어야 함

constexpr int SIZE_DEFAULT = 15;
class BordManager : public Singleton<BordManager>
{
	friend class Singleton<BordManager>;
	BordManager() = default;
	~BordManager() = default;

	array<array<shared_ptr<Stone>, SIZE_DEFAULT>, SIZE_DEFAULT> m_board; // 바둑판

	struct Position { int row; int col; };
	unordered_map<Position, StoneType> m_stoneTypeMap; // 위치별 돌 종류 맵
	vector<pair<Position, StoneAbility>> m_jokerPositions; // 조커 돌 위치와 능력

	StoneType m_stoneType = StoneType::White;
	StoneAbility m_stoneAbility = StoneAbility::None;

	void PlaceStone(int row, int col, StoneType stoneType, StoneAbility stoneAbility);
	shared_ptr<Stone> GetStone(int row, int col);
};