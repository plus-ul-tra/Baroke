#include "pch.h"
#include "BoardManager.h"

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

void BoardManager::Initialize()
{
	// 바둑판 초기화
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
	m_stoneType = StoneType::White; // 기본 돌 타입은 흰 돌
	m_stoneAbility = StoneAbility::None; // 기본 돌 능력은 없음
}

void BoardManager::PlaceRandomStones(int amount)
{
	// 아직 미구현
}

void BoardManager::InputBasedGameLoop(POINT mousePos)
{
	m_selectedPosition = MouseToBoardPosition(mousePos);
	// 만약 돌은 놓기가 실패했다면 리턴
	if (!PlaceStone(m_selectedPosition, m_stoneType, m_stoneAbility)) return;

	JokerAbilityUpdate(); // 조커 능력 업데이트

	// 사석 판정

	m_selectedPosition = { -1, -1 }; // 마지막으로 선택된 위치 초기화
	m_stoneType = StoneType::White; // 돌 타입 초기화
	m_stoneAbility = StoneAbility::None; // 돌 능력 초기화
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
		m_board[selectedPosition.x][selectedPosition.y] = make_shared<WhiteStone>(BoardToScreenPosition(selectedPosition), 50.0f);
	}
	else
	{
		if (stoneAbility == StoneAbility::None)
		{
			// 만약 능력이 없는 흑돌이라면 일반 흑돌 생성
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<BlackStone>(BoardToScreenPosition(selectedPosition), 50.0f);
		}
		else
		{
			// 능력이 있디면 조커 돌로 생성 // 흑돌이지만 능력이 있는 경우도 조커 돌로 생성
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<JokerStone>(BoardToScreenPosition(selectedPosition), 50.0f, stoneAbility);
			// 능력이 있는 조커 돌의 경우 능력 벡터에 추가
			m_jokerPositions.emplace_back(selectedPosition, stoneAbility);
		}
	}

	// 사석 판정용 돌 타입 저장 // JokerStone이라도 StoneType::Black 이라면 흑돌로 저장
	m_stoneTypeMap[selectedPosition] = stoneType;

	return true;
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