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
	StoneType m_stoneType = StoneType::Black;
	StoneAbility m_stoneAbility = StoneAbility::None;

	int m_offX, m_offY, m_cell, m_padding, m_stoneOffset;
public:
	void Initialize(int offX, int offY, int _cell,int _stoneOffset, int padding = 0);
	void PlaceRandomStones(int amount); // 게임 시작 시 랜덤으로 돌을 놓기 위한 함수 // 아직 미구현


	bool InputBasedGameLoop(POINT mousePos); // 클릭으로 돌 놓기
	bool InputBasedGameLoop(int row, int col); // 배열에 접근으로 돌 놓기

	void JokerAbilityUpdate(); // 조커 능력 실행

	// 마우스 위치와 바둑판 위치 변환 함수 아직 미구현
	POINT MouseToBoardPosition(POINT mousePos) const {
		return { (mousePos.x - m_offX - m_padding + m_cell / 2) / m_cell,
				 (mousePos.y - m_offY - m_padding + m_cell / 2) / m_cell };
	}
	POINT BoardToScreenPosition(POINT boardPos) const  // row, col
	{ 
		return
		{
			m_offX + m_padding + boardPos.x * m_cell,
			m_offY + m_padding + boardPos.y * m_cell
		};
	}


	bool PlaceStone(POINT selectedPosition, StoneType stoneType, StoneAbility stoneAbility);

	void ResetStone();
	shared_ptr<Stone> GetStone(POINT);
	Board GetBoard() const { return m_board; }
	int GetBoardSize() const { return SIZE_DEFAULT; }
	unordered_map<POINT, StoneType> GetStoneTypeMap() {	return  m_stoneTypeMap;	}

	// 돌 변경 함수
	void SetStoneType(StoneType type) { m_stoneType = type; }
	void SetStoneAbility(StoneAbility ability) { m_stoneAbility = ability; }


private:
	int CountLiberty(int r, int c,
		std::vector<POINT>& group,
		std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT>& visited) const;
	void RemoveGroup(const std::vector<POINT>& group);
};