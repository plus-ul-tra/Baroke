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

	Board m_board; // 바둑판
	unordered_map<POINT, StoneType> m_stoneTypeMap; // 위치별 돌 종류 맵 // 사석 판정용

	vector<pair<POINT, StoneAbility>> m_jokerPositions; // 조커 돌 위치와 능력 벡터

	POINT m_selectedPosition = { -1, -1 }; // 인풋으로 선택된 바둑판 위치
	StoneType m_stoneType = StoneType::White;
	StoneAbility m_stoneAbility = StoneAbility::None;

public:
	void Initialize();
	void PlaceRandomStones(int amount); // 게임 시작 시 랜덤으로 돌을 놓기 위한 함수 // 아직 미구현

	void InputBasedGameLoop(POINT mousePos); // 클릭으로 돌 놓기
	void JokerAbilityUpdate(); // 조커 능력 실행

	// 마우스 위치와 바둑판 위치 변환 함수 아직 미구현
	POINT MouseToBoardPosition(POINT mousePos) const { return { mousePos.x / 50, mousePos.y / 50 }; }
	POINT BoardToScreenPosition(POINT boardPos) const { return { boardPos.x * 50, boardPos.y * 50 }; }

	bool PlaceStone(POINT selectedPosition, StoneType stoneType, StoneAbility stoneAbility);

	shared_ptr<Stone> GetStone(POINT);
	Board GetBoard() const { return m_board; }

	// 돌 변경 함수
	void SetStoneType(StoneType type) { m_stoneType = type; }
	void SetStoneAbility(StoneAbility ability) { m_stoneAbility = ability; }
};