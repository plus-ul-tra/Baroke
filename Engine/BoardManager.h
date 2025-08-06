#pragma once
#include "Singleton.h"
#include "Stone.h"
#include "PlayerInfo.h"
#include "ModeInfo.h"

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
	friend struct JokerFunctionsWrapper;

	BoardManager() = default;
	~BoardManager() = default;

	Board m_board; // 바둑판
	unordered_map<POINT, StoneType> m_stoneTypeMap; // 위치별 돌 종류 맵 // 사석 판정용
	vector<pair<POINT, StoneAbility>> m_jokerPositions; // 조커 돌 위치와 능력 벡터


	POINT m_selectedPosition = { -1, -1 }; // 인풋으로 선택된 바둑판 위치
	StoneType m_stoneType = StoneType::Black;
	StoneAbility m_stoneAbility = StoneAbility::None;


	// Initialize 변수들
	int m_offX = 0, m_offY = 0, m_drawW = 0, m_drawH = 0, m_cell = 0, m_padding = 0, m_stoneOffset = 0;

	void InitializeJokerInfoMap();

	int CountLiberty // 사석 판정용 함수
	(
		int r, int c,
		std::vector<POINT>& group,
		std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT>& visited
	) const;

	size_t WhiteStoneRemoveCheck(POINT selectedPosition);
	void RemoveGroup(const std::vector<POINT>& group); // 사석 판정된 돌 그룹 제거 함수
	void CheckRemovedStones();
	void RemoveJokerStone(POINT position); // 조커 돌 제거 함수

public:
	PlayerInfo m_playerInfo; // 플레이어 정보 // 흑돌, 백돌, 돈 등

	void Initialize(int offX, int offY, int drawW, int drawH, int _cell, int _stoneOffset, int padding = 0);
	void PlaceRandomStones(int amount); // 게임 시작 시 랜덤으로 돌을 놓기 위한 함수

	bool InputBasedGameLoop(POINT mousePos); // 클릭으로 돌 놓기
	bool InputBasedGameLoop(int row, int col); // 배열에 접근으로 돌 놓기

	void JokerAbilityUpdate(); // 모든 조커 능력 실행
	void JokerAbillityUse(StoneAbility ab, POINT position);

	POINT MouseToBoardPosition(POINT mousePos) const; // 마우스 좌표를 바둑판 좌표로 변환
	POINT BoardToScreenPosition(POINT boardPos) const; // 바둑판 좌표를 스크린 좌표로 변환

	bool isValidPoint(POINT position) const { return position.x >= 0 && position.x < SIZE_DEFAULT && position.y >= 0 && position.y < SIZE_DEFAULT; }
	bool PlaceStone(POINT selectedPosition, StoneType stoneType, StoneAbility stoneAbility);
	void ResetStone();

	shared_ptr<Stone> GetStone(POINT);
	bool IsJokerStone(POINT position) const;

	Board GetBoard() const { return m_board; }
	int GetBoardSize() const { return SIZE_DEFAULT; }

	unordered_map<POINT, StoneType> GetStoneTypeMap() {	return  m_stoneTypeMap;	}
	int GetStoneTypeAmount(StoneType type) const;

	// 다음 착수 돌 변경 함수
	void SetStoneType(StoneType type) { m_stoneType = type; }
	void SetStoneAbility(StoneAbility ability) { m_stoneAbility = ability; }


	//---------------------------------------------------------------- 희생 모드 진입, 탈출
public:
	void SetMode(UIMode uimode) { nowMode = uimode; }
	void ExitMode() { 
		nowMode = UIMode::Normal;
		m_stoneType = StoneType::Black;
		m_stoneAbility = StoneAbility::None;
		ResetGroup(); 
		std::cout << "exit" << std::endl; }

	UIMode GetMode() { return nowMode; }
	const std::vector<POINT>& GetSacrificeGroup() const { return m_SacrificeGroup; }

	bool SelectSacrificeStone(POINT mousePos); 
	bool checkSacrificeSuccess();

	StoneAbility GetStoneAbillity() const { return m_stoneAbility; } ;
	StoneType GetStoneType() const { return m_stoneType; };
	void SyncBlackStoneCount(int count) { m_syncBlackStoneCount = count; }

	int GetsyncBlackStoneCount() { return m_syncBlackStoneCount; }
private:
	int m_syncBlackStoneCount = 0;
	std::vector<POINT> m_SacrificeGroup; // 희생모드에서 선택한 그룹임
	void ResetGroup() {
		m_SacrificeGroup.clear(); 
		m_useCondGroup.clear();
	};
	bool m_sacrificeMode = false;
	UIMode nowMode = UIMode::Normal;
	//---------------------------------------------------------------- 조건 충족 모드 진입, 탈출
public:
	bool checkBeforeAbSuccess();
	bool SelectUseCond(POINT mousePos);
	const std::vector<POINT>& GetuseCondGroup() const { return m_useCondGroup; }
private:
	std::vector<POINT> m_useCondGroup; // 조건 충족모드에서 선택한 그룹임
	POINT direction = { 0,0 }; // 상 = 0,1 // 하 = 0,-1 // 좌 = -1,0 // 우 = 1,0
	//---------------------------------------------------------------- 버튼 - 상태 판정용 함수
public:
	int  CountStones(StoneType t) const;					// 보드판위에 특정돌이 몇갠지 체크
	bool HasStraightLine(StoneType type, int len) const; // 직선 범위 체크
	bool WhiteLibOne() const;							// 자유도1인 흰돌 체크
	bool IsColorCount(StoneType type, int need ) const; // 착수했을때 3*3범위에 특정 컬러 돌이 몇개 있는 위치가 있는지 체크
	bool IsSamaBlackWhite() const;						// 착수했을때 3*3범위에 흑돌흰돌 개수 같은지 체크
	bool IsLibZero() const;								// 
	bool IsConnectTwo() const;
	bool IsOthello() const;
};