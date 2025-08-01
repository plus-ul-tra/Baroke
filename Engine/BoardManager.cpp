#include "pch.h"
#include "BoardManager.h"


static constexpr int DR[4] = { -1, 1, 0, 0 };
static constexpr int DC[4] = { 0, 0,-1, 1 };

inline bool IsOnBoard(int r, int c)
{
	return r >= 0 && r < SIZE_DEFAULT && c >= 0 && c < SIZE_DEFAULT;
}

// 돌 능력에 대한 함수 맵 // 해당하는 조커와 조커의 위치를 받음 // 나중에 다른 위치로 이동할 수도 있음
static unordered_map<StoneAbility, function<void(shared_ptr<JokerStone>)>> g_abilityFunctions =
{
	{ StoneAbility::JokerAbility1, [](shared_ptr<JokerStone> jokerStone)
	{
		// 조커 능력 1
		cout << jokerStone->GetPosition().x << ", " << jokerStone->GetPosition().y << endl;
	} },

	{ StoneAbility::JokerAbility2, [](shared_ptr<JokerStone> jokerStone)
	{
		// 조커 능력 2
		cout << jokerStone->GetPosition().x << ", " << jokerStone->GetPosition().y << endl;
	} },

	{ StoneAbility::JokerAbility3, [](shared_ptr<JokerStone> jokerStone)
	{
		// 조커 능력 3
		cout << jokerStone->GetPosition().x << ", " << jokerStone->GetPosition().y << endl;
	} }
};

void BoardManager::Initialize(int offX, int offY, int drawW, int drawH, int _cell, int _stoneOffset, int padding)
{
	m_offX = offX;
	m_offY = offY;
	m_drawW = (drawW / 2) - padding;
	m_drawH = (drawH / 2) - padding;
	m_cell = _cell;
	m_padding = padding;
	m_stoneOffset = _stoneOffset;
	// 바둑판 초기화
	ResetStone();
}

void BoardManager::PlaceRandomStones(int amount)
{
	int N = SIZE_DEFAULT;
	std::vector<std::pair<int, int>> allPositions;

	for (int r = 0; r < N; ++r)
		for (int c = 0; c < N; ++c)
			allPositions.emplace_back(r, c);

	std::shuffle(allPositions.begin(), allPositions.end(), std::mt19937(std::random_device{}()));


	int n = std::min(static_cast<int>(allPositions.size()), amount);

	for (int i = 0; i < n; ++i)
	{
		int r = allPositions[i].first;
		int c = allPositions[i].second;
		PlaceStone({ r, c }, StoneType::White, StoneAbility::None);
	}
}


bool BoardManager::InputBasedGameLoop(POINT mousePos)
{
	m_selectedPosition = MouseToBoardPosition(mousePos);
	std::cout << m_selectedPosition.x << " " << m_selectedPosition.y << std::endl;
	// 만약 돌은 놓기가 실패했다면 리턴
	if (!PlaceStone(m_selectedPosition, m_stoneType, m_stoneAbility)) return false;

	JokerAbilityUpdate(); // 조커 능력 업데이트

	m_selectedPosition = { -1, -1 }; // 마지막으로 선택된 위치 초기화
	m_stoneType = StoneType::Black; // 돌 타입 초기화
	m_stoneAbility = StoneAbility::None; // 돌 능력 초기화

	return true;
}

bool BoardManager::InputBasedGameLoop(int row, int col) // row , col 입력 받아서 해당 배열에 액세스 해서 넣으면댐
{

/*	m_selectedPosition = BoardToScreenPosition({ row,col });*/

	std::cout <<"row , col = " << row << " " << col << std::endl;
	// 만약 돌은 놓기가 실패했다면 리턴
	if (!PlaceStone({row,col}, m_stoneType, m_stoneAbility)) return false;

	JokerAbilityUpdate(); // 조커 능력 업데이트


	m_selectedPosition = { -1, -1 }; // 마지막으로 선택된 위치 초기화
	m_stoneType = StoneType::Black; // 돌 타입 초기화
	m_stoneAbility = StoneAbility::None; // 돌 능력 초기화

	return true;
}

void BoardManager::JokerAbilityUpdate()
{
	for (const auto& joker : m_jokerPositions)
	{
		POINT position = joker.first; // 조커 돌의 위치
		shared_ptr<Stone> stone = GetStone(position); // 해당 위치의 Stone(Object) 가져오기

		StoneAbility ability = joker.second; // 조커 돌의 능력

		shared_ptr<JokerStone> jokerStone = dynamic_pointer_cast<JokerStone>(stone); // Stone을 JokerStone으로 캐스팅

		auto it = g_abilityFunctions.find(ability);
		if (it != g_abilityFunctions.end())
		{
			it->second(jokerStone);
		}
	}
}



POINT BoardManager::MouseToBoardPosition(POINT mousePos) const
{
	return
	{
		(mousePos.x+ m_drawW + m_cell/2)/ m_cell,
		-(mousePos.y - m_drawH - m_cell / 2) / m_cell
//  		(mousePos.x - m_offX - m_padding + m_cell / 2) / m_cell,
//  		(mousePos.y - m_offY - m_padding + m_cell / 2) / m_cell
	};
}

// 보드 인덱스(row, col) → 화면(픽셀)
POINT BoardManager::BoardToScreenPosition(POINT boardPos) const
{
	return
	{
		boardPos.x*m_cell- m_drawW - m_cell / 2,
		-boardPos.y*m_cell + m_drawH - m_cell / 2
//  		m_offX + m_padding + boardPos.x * m_cell,
//  		m_offY + m_padding + boardPos.y * m_cell
	};
}

bool BoardManager::PlaceStone(POINT selectedPosition, StoneType stoneType, StoneAbility stoneAbility)
{
	if (selectedPosition.x < 0 || selectedPosition.x >= SIZE_DEFAULT || selectedPosition.y < 0 || selectedPosition.y >= SIZE_DEFAULT)
	{
		std::cout << "유효하지 않은 위치: (" << selectedPosition.x << ", " << selectedPosition.y << ")" << std::endl;
		return false;
	}
	if (m_board[selectedPosition.x][selectedPosition.y])
	{
		std::cout << "이미 돌이 있는 위치: (" << selectedPosition.x << ", " << selectedPosition.y << ")" << std::endl;
		return false;
	}

	if (stoneType == StoneType::White)
	{
		// 흰 돌 생성 // row, col, size는 나중에 스크린에 맞는 좌표로 변경해야함
		m_board[selectedPosition.x][selectedPosition.y] = make_shared<WhiteStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset);
	}
	else
	{
		if (stoneAbility == StoneAbility::None)
		{
			// 만약 능력이 없는 흑돌이라면 일반 흑돌 생성
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<BlackStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset);
		}
		else
		{
			// 능력이 있디면 조커 돌로 생성 // 흑돌이지만 능력이 있는 경우도 조커 돌로 생성
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<JokerStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset, stoneAbility);
			// 능력이 있는 조커 돌의 경우 능력 벡터에 추가
			m_jokerPositions.emplace_back(selectedPosition, stoneAbility);
		}
	}

	// 사석 판정용 돌 타입 저장 // JokerStone이라도 StoneType::Black 이라면 흑돌로 저장
	m_stoneTypeMap[selectedPosition] = stoneType;


	for (int k = 0; k < 4; ++k) {
		int nr = selectedPosition.x + DR[k], nc = selectedPosition.y + DC[k];
		if (!IsOnBoard(nr, nc)) continue;

		auto it = m_stoneTypeMap.find({ nr, nc });
		if (it == m_stoneTypeMap.end() || it->second == StoneType::Black || it->second == StoneType::Joker) continue;
		std::vector<POINT> grp;
		std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> vis{};
		if (CountLiberty(nr, nc, grp, vis) == 0)
			RemoveGroup(grp);          // 흰 돌만 제거
	}

	return true;
}

void BoardManager::ResetStone()
{
	for (int i = 0; i < SIZE_DEFAULT; ++i)
	{
		for (int j = 0; j < SIZE_DEFAULT; ++j)
		{
			m_board[i][j] = nullptr; // 모든 위치를 nullptr로 초기화
		}
	}
	m_stoneTypeMap.clear(); // 돌 종류 맵 초기화
	m_jokerPositions.clear(); // 조커 돌 위치와 능력 벡터 초기화
	m_selectedPosition = { -1, -1 }; // 선택된 위치 초기화
	m_stoneType = StoneType::Black; // 기본 돌 타입은 흰 돌
	m_stoneAbility = StoneAbility::None; // 기본 돌 능력은 없음
}

shared_ptr<Stone> BoardManager::GetStone(POINT position)
{
	if (position.x < 0 || position.x >= SIZE_DEFAULT || position.y < 0 || position.y >= SIZE_DEFAULT)
	{
		std::cout << "유효하지 않은 위치: (" << position.x << ", " << position.y << ")" << std::endl;
		return nullptr;
	}

	return m_board[position.x][position.y];
}

bool BoardManager::IsJokerStone(POINT position) const
{
	if (m_jokerPositions.size() > 0) return false;

	for (const auto& joker : m_jokerPositions) // 조커 돌 위치와 능력 벡터를 순회
	{
		if (position == joker.first)
		{
			return true;
		}
	}

	return false;
}

int BoardManager::GetStoneTypeAmount(StoneType type) const
{
	int count = 0;
	for (const auto& pair : m_stoneTypeMap)
	{
		if (pair.second == type && !IsJokerStone(pair.first))
		{
			++count;
		}
	}

	return count;
}

int BoardManager::CountLiberty(
	int r, int c,
	std::vector<POINT>& group,
	std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT>& visited) const
{
	auto it0 = m_stoneTypeMap.find({ r,c });
	if (it0 == m_stoneTypeMap.end()) return 0;
	StoneType target = it0->second;

	std::stack<POINT> st;  st.push({ r,c });
	visited[r][c] = true;
	int libs = 0;

	while (!st.empty()) {
		auto [cr, cc] = st.top(); st.pop();
		group.push_back({ cr,cc });

		for (int k = 0; k < 4; ++k) {
			int nr = cr + DR[k], nc = cc + DC[k];
			if (!IsOnBoard(nr, nc)) continue;

			auto it = m_stoneTypeMap.find({ nr,nc });
			if (it == m_stoneTypeMap.end()) { ++libs; continue; }  // 상하 좌우 체크 후 흰돌을 못 찾은 경우
			if (it->second ==StoneType::Joker) { ++libs; continue; }  // 상하 좌우 체크 후 흰돌을 못 찾은 경우
			if (it->second == target && !visited[nr][nc]) {			// 상하 좌우 체크 후 흰돌을 찾은 경우
				visited[nr][nc] = true;  st.push({ nr,nc });
			}
		}
	}
	return libs;
}

void BoardManager::RemoveGroup(const std::vector<POINT>& g)
{
	for (POINT p : g) {
		m_board[p.x][p.y] = nullptr;
		m_stoneTypeMap.erase(p);
	}
}
