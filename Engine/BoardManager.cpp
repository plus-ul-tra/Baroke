#include "pch.h"
#include "BoardManager.h"
#include "SceneManager.h"

static constexpr int DR[4] = { -1, 1, 0, 0 };
static constexpr int DC[4] = { 0, 0,-1, 1 };

// 돌 능력에 대한 함수 맵 // 해당하는 조커와 조커의 위치를 받음 // 나중에 다른 위치로 이동할 수도 있음
struct JokerFunctionsWrapper
{

	BoardManager& boardManager = BoardManager::GetInstance();

	unordered_map<StoneAbility, function<void(shared_ptr<JokerStone>, POINT)>> g_abilityFunctions =
	{
		{ StoneAbility::jokerEgg, [this](shared_ptr<JokerStone> jokerEgg, POINT position)
		{
			if (!jokerEgg->m_jokerInfo.coolTime)
			{
				boardManager.m_playerInfo.m_BlackStone += jokerEgg->m_jokerInfo.functionVariable;
				jokerEgg->m_jokerInfo.coolTime = m_jokerInfoMap.find(StoneAbility::jokerEgg)->second.coolTime; // 쿨타임 초기화
			}
			jokerEgg->m_jokerInfo.coolTime--;

			std::cout << boardManager.m_playerInfo.m_BlackStone << std::endl;
		}
		},
		{ StoneAbility::jokerOstrichEgg, [this](shared_ptr<JokerStone> jokerOstrichEgg, POINT position)
		{
			if (!jokerOstrichEgg->m_jokerInfo.coolTime)
			{
				boardManager.m_playerInfo.m_BlackStone += jokerOstrichEgg->m_jokerInfo.functionVariable;
				jokerOstrichEgg->m_jokerInfo.coolTime = m_jokerInfoMap.find(StoneAbility::jokerOstrichEgg)->second.coolTime; // 쿨타임 초기화
			}
			jokerOstrichEgg->m_jokerInfo.coolTime--;

			std::cout << boardManager.m_playerInfo.m_BlackStone << std::endl;
		}
		},
		{ StoneAbility::jokerDansu, [this](shared_ptr<JokerStone> jokerBite, POINT positon)
		{
			// 이건는 방향을 인풋으로 받아야 함
		}
		},
		{ StoneAbility::jokerPeacock, [this](shared_ptr<JokerStone> jokerPeacock, POINT position)
		{
			if (jokerPeacock->m_jokerInfo.coolTime != 0) return;
			jokerPeacock->m_jokerInfo.coolTime--;

			if (boardManager.GetStoneTypeAmount(White) >= 5)
			{
				int patternX[8] = { 0, -1, 1, -1, 1, -1, 0, 1 };
				int patternY[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

				int functionVariable = jokerPeacock->m_jokerInfo.functionVariable;

				int count = 0;
				for (const auto& pair : boardManager.GetStoneTypeMap())
				{
					if (count >= 8) break;
					if (pair.second != StoneType::White) continue;

					if (abs(pair.first.x - position.x) <= functionVariable &&
						abs(pair.first.y - position.y) <= functionVariable &&
						(abs(pair.first.x - position.x) > 1 || abs(pair.first.y - position.y) > 1))
					{
						while (boardManager.isValidPoint({ position.x + patternX[count], position.y + patternY[count] }))
						{
							std::cout << "Moving stone at (" << pair.first.x << ", " << pair.first.y << ") to new position: " << std::endl;
							std::cout << "(" << position.x + patternX[count] << ", " << position.y + patternY[count] << ")" << std::endl;

							if (boardManager.m_board[position.x + patternX[count]][position.y + patternY[count]])
							{
								count++;
								if (count >= 8) break;
								continue;
							}

							boardManager.m_board[pair.first.x][pair.first.y]->Move
							(
								boardManager.BoardToScreenPosition({ position.x + patternX[count], position.y + patternY[count] }), 1
							);

							boardManager.m_board[position.x + patternX[count]][position.y + patternY[count]] = boardManager.m_board[pair.first.x][pair.first.y];
							boardManager.m_board[pair.first.x][pair.first.y] = nullptr;

							boardManager.m_stoneTypeMap[{ position.x + patternX[count], position.y + patternY[count] }] = boardManager.m_stoneTypeMap[pair.first];
							boardManager.m_stoneTypeMap.erase(pair.first);

							break;
						}
					}
				}
			}
		}
		},

		{ StoneAbility::jokerExplode, [this](shared_ptr<JokerStone> jokerExplode, POINT position)
		{
			if (jokerExplode->m_jokerInfo.lifeSpan <= 0) boardManager.RemoveGroup({position});

			int patternX[8] = { -1, 0, 1, 1, -1, -1, 0, 1 };
			int patternY[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

			for (int i = 0; i < 8; ++i)
			{
				int x = position.x + patternX[i];
				int y = position.y + patternY[i];

				if (boardManager.isValidPoint({ x, y }) && boardManager.m_board[x][y])
				{
					if (boardManager.m_stoneTypeMap.find({ x, y })->second = StoneType::Black)
					{
						boardManager.m_playerInfo.m_BlackStone++;
					}

					boardManager.RemoveGroup({ { x, y } });
				}
			}
			jokerExplode->m_jokerInfo.lifeSpan--;
		}
		},
		{ StoneAbility::jokerBlackhole, [this](shared_ptr<JokerStone> jokerBlackhole, POINT position)
		{
			 // Mediator에 위치 설정
			if (jokerBlackhole->m_jokerInfo.lifeSpan <= 0) boardManager.RemoveGroup({position});
			if (jokerBlackhole->m_jokerInfo.coolTime != 0) return;

			int functionVariable = jokerBlackhole->m_jokerInfo.functionVariable;
			for (const auto& pair : boardManager.GetStoneTypeMap())
			{
				if (abs(pair.first.x - position.x) <= functionVariable && abs(pair.first.y - position.y) <= functionVariable) // && (abs(pair.first.x - position.x) > 0 || abs(pair.first.y - position.y) > 0))
				{
					Mediator::GetInstance().SetPosition(jokerBlackhole->GetPosition());
					SceneManager::GetInstance().ChangePostProcessing("BlackHole"); // 필터 적용
				
					boardManager.m_board[pair.first.x][pair.first.y]->Move(boardManager.BoardToScreenPosition({ position.x, position.y }), 3);
					boardManager.m_board[pair.first.x][pair.first.y]->Remove(3);

					if (boardManager.m_stoneTypeMap.find(pair.first)->second == StoneType::Black)
					{
						boardManager.m_playerInfo.m_BlackStone++;
					}
					else if (boardManager.m_stoneTypeMap.find(pair.first)->second == StoneType::White)
					{
						boardManager.m_playerInfo.m_money++;
					}
					boardManager.m_stoneTypeMap.erase(pair.first);
				}
			}
			jokerBlackhole->m_jokerInfo.lifeSpan--;
			jokerBlackhole->m_jokerInfo.coolTime--;
		}
		},

		{ StoneAbility::jokerFusion, [this](shared_ptr<JokerStone> jokerFusion, POINT position)
		{
			vector<POINT> grp;
			array<array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> vis{};
			if (boardManager.CountLiberty(position.x, position.y, grp, vis) == 0)
			{
				boardManager.m_playerInfo.m_BlackStone += jokerFusion->m_jokerInfo.functionVariable * grp.size();
			}

			std::cout << boardManager.m_playerInfo.m_BlackStone << std::endl;
		}
		},

		{ StoneAbility::jokerTriunion, [this](shared_ptr<JokerStone> jokerTriunion, POINT position)
		{
			vector<POINT> grp;
			array<array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> vis{};
			if (boardManager.CountLiberty(position.x, position.y, grp, vis) == 0)
			{
				boardManager.m_playerInfo.m_BlackStone += jokerTriunion->m_jokerInfo.functionVariable * grp.size();
			}

			std::cout << boardManager.m_playerInfo.m_BlackStone << std::endl;
		}
		},
		{ StoneAbility::jokerQuadunion, [this](shared_ptr<JokerStone> jokerQuadunion, POINT position)
		{
			vector<POINT> grp;
			array<array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> vis{};
			if (boardManager.CountLiberty(position.x, position.y, grp, vis) == 0)
			{
				boardManager.m_playerInfo.m_BlackStone += jokerQuadunion->m_jokerInfo.functionVariable * grp.size();
				boardManager.RemoveJokerStone(position);
			}

			std::cout << boardManager.m_playerInfo.m_BlackStone << std::endl;
		}
		},


		{ StoneAbility::jokerSplit, [this](shared_ptr<JokerStone> jokerSplit, POINT position)
		{
			if (!jokerSplit->m_jokerInfo.coolTime)
			{
				std::random_device rd;
				mt19937 rng(rd());
				uniform_int_distribution<int> dist(1, 100);

				if (dist(rng) <= jokerSplit->m_jokerInfo.functionVariable)
				{
					for (int i = 0; i < 10000; ++i)
					{
						uniform_int_distribution<int> distX(0, SIZE_DEFAULT - 1);
						uniform_int_distribution<int> distY(0, SIZE_DEFAULT - 1);
						int randomX = distX(rng);
						int randomY = distY(rng);

						if (boardManager.PlaceStone({ randomX, randomY }, StoneType::Black, StoneAbility::None)) break;
					}
				}

				jokerSplit->m_jokerInfo.coolTime = m_jokerInfoMap.find(StoneAbility::jokerSplit)->second.coolTime;
			}
			jokerSplit->m_jokerInfo.coolTime--;

		}
		},
		{ StoneAbility::jokerWaxseal, [this](shared_ptr<JokerStone> jokerWaxseal, POINT position)
		{
			boardManager.m_playerInfo.m_waxMoney += (boardManager.WhiteStoneRemoveCheck(position) * jokerWaxseal->m_jokerInfo.functionVariable);

			std::cout << "Wax Money : " << boardManager.m_playerInfo.m_waxMoney << std::endl;
		}
		},

		{ StoneAbility::jokerWind, [this](shared_ptr<JokerStone> jokerWind, POINT position)
		{
			if (jokerWind->m_jokerInfo.lifeSpan <= 0) boardManager.RemoveGroup({position});

			if (!jokerWind->m_jokerInfo.coolTime)
			{
				std::random_device rd;
				mt19937 rng(rd());
				uniform_int_distribution<int> dist(0, 3);
				int randomDirectionIndex = dist(rng);

				POINT randomDirection = { position.x + DR[randomDirectionIndex], position.y + DC[randomDirectionIndex] };
				if (!boardManager.isValidPoint(randomDirection) || !boardManager.m_board[randomDirection.x][randomDirection.y]) return;

				POINT targetPosition = { position.x + (DR[randomDirectionIndex] * (1 + jokerWind->m_jokerInfo.functionVariable)), position.y + (DC[randomDirectionIndex] * (1 + jokerWind->m_jokerInfo.functionVariable)) };
				if (boardManager.isValidPoint(targetPosition) && !boardManager.m_board[targetPosition.x][targetPosition.y])
				{
					boardManager.m_board[randomDirection.x][randomDirection.y]->Move(boardManager.BoardToScreenPosition(targetPosition), 1);
					boardManager.m_board[targetPosition.x][targetPosition.y] = boardManager.m_board[randomDirection.x][randomDirection.y];
					boardManager.m_board[randomDirection.x][randomDirection.y] = nullptr;

					boardManager.m_stoneTypeMap[targetPosition] = boardManager.m_stoneTypeMap[randomDirection];
					boardManager.m_stoneTypeMap.erase(randomDirection);
				}

				jokerWind->m_jokerInfo.coolTime = m_jokerInfoMap.find(StoneAbility::jokerSplit)->second.coolTime;
			}
			jokerWind->m_jokerInfo.coolTime--;
			jokerWind->m_jokerInfo.lifeSpan--;
		}
		},
	};
};

void BoardManager::Initialize(int offX, int offY, int drawW, int drawH, int _cell, int _stoneOffset, int padding)
{
	InitializeJokerInfoMap(); // 조커 능력 정보 초기화

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


bool BoardManager::InputBasedGameLoop(POINT mousePos) // 마우스 클릭으로 돌 놓기
{
	m_selectedPosition = MouseToBoardPosition(mousePos);

	if (!PlaceStone(m_selectedPosition, m_stoneType, m_stoneAbility)) return false;
	JokerAbilityUpdate(); // 조커 능력 업데이트
	WhiteStoneRemoveCheck(m_selectedPosition); // 흰 돌 체크
	CheckRemovedStones();

	m_selectedPosition = { -1, -1 }; // 마지막으로 선택된 위치 초기화
	m_stoneType = StoneType::Black; // 돌 타입 초기화
	m_stoneAbility = StoneAbility::None; // 돌 능력 초기화

	return true;
}

bool BoardManager::InputBasedGameLoop(int row, int col) // 바둑판 기준 row , col 입력 받아서 해당 배열에 액세스 해서 넣으면댐
{
	if (!PlaceStone({ row,col }, m_stoneType, m_stoneAbility)) return false;
	JokerAbilityUpdate(); // 조커 능력 업데이트
	WhiteStoneRemoveCheck(m_selectedPosition); // 흰 돌 체크
	CheckRemovedStones();

	m_selectedPosition = { -1, -1 }; // 마지막으로 선택된 위치 초기화
	m_stoneType = StoneType::Black; // 돌 타입 초기화
	m_stoneAbility = StoneAbility::None; // 돌 능력 초기화

	return true;
}

void BoardManager::JokerAbilityUpdate()
{
	JokerFunctionsWrapper wrapper;
	for (const auto& joker : m_jokerPositions)
	{
		POINT position = joker.first; // 조커 돌의 위치
		shared_ptr<Stone> stone = GetStone(position); // 해당 위치의 Stone(Object) 가져오기

		StoneAbility ability = joker.second; // 조커 돌의 능력

		shared_ptr<JokerStone> jokerStone = dynamic_pointer_cast<JokerStone>(stone); // Stone을 JokerStone으로 캐스팅

		auto it = wrapper.g_abilityFunctions.find(ability);
		if (it != wrapper.g_abilityFunctions.end())
		{
			it->second(jokerStone, position); // 조커 능력 함수 실행
		}
	}
}

POINT BoardManager::MouseToBoardPosition(POINT mousePos) const
{
	return
	{
		(mousePos.x + m_drawW + m_cell / 2) / m_cell,
		-(mousePos.y - m_drawH - m_cell / 2) / m_cell
//  		(mousePos.x - m_offX - m_padding + m_cell / 2) / m_cell,
//  		(mousePos.y - m_offY - m_padding + m_cell / 2) / m_cell
	};
}


POINT BoardManager::BoardToScreenPosition(POINT boardPos) const
{
	return
	{
		boardPos.x * m_cell- m_drawW - m_cell / 2,
		-boardPos.y * m_cell + m_drawH - m_cell / 2
//  		m_offX + m_padding + boardPos.x * m_cell,
//  		m_offY + m_padding + boardPos.y * m_cell
	};
}

bool BoardManager::PlaceStone(POINT selectedPosition, StoneType stoneType, StoneAbility stoneAbility)
{
	if (!isValidPoint(selectedPosition))
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
		if (stoneAbility == StoneAbility::None)
		{
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<WhiteStone>(BoardToScreenPosition(selectedPosition), m_cell, m_stoneOffset);
		}
		else
		{
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<JokerStone>(BoardToScreenPosition(selectedPosition), m_cell, m_stoneOffset, stoneAbility);

			m_jokerPositions.emplace_back(selectedPosition, stoneAbility);
		}
	}
	else
	{
		if (stoneAbility == StoneAbility::None)
		{
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<BlackStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset);
		}
		else
		{
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<JokerStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset, stoneAbility);

			m_jokerPositions.emplace_back(selectedPosition, stoneAbility);
		}
	}

	// 사석 판정용 돌 타입 저장 // JokerStone이라도 StoneType::Black 이라면 흑돌로 저장
	m_stoneTypeMap[selectedPosition] = stoneType;

	return true;
}

size_t BoardManager::WhiteStoneRemoveCheck(POINT selectedPosition)
{
	size_t groupSize = 0;

	for (int k = 0; k < 4; ++k) {
		int nr = selectedPosition.x + DR[k], nc = selectedPosition.y + DC[k];
		if (!isValidPoint({ nr, nc })) continue;

		auto it = m_stoneTypeMap.find({ nr, nc });
		if (it == m_stoneTypeMap.end() || it->second == StoneType::Black || it->second == StoneType::Joker) continue;
		std::vector<POINT> grp;
		std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> vis{};
		if (CountLiberty(nr, nc, grp, vis) == 0)
		{
			groupSize += grp.size();
			RemoveGroup(grp);
		}
	}

	m_playerInfo.m_money += groupSize;
	return groupSize;
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
	if (!isValidPoint(position))
	{
		std::cout << "유효하지 않은 위치: (" << position.x << ", " << position.y << ")" << std::endl;
		return nullptr;
	}

	return m_board[position.x][position.y];
}

bool BoardManager::IsJokerStone(POINT position) const
{
	if (m_jokerPositions.size() < 0) return false;

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
		if (type == StoneType::Joker)
		{
			if (IsJokerStone(pair.first))
			{
				++count;
			}
			continue;
		}
		if (!IsJokerStone(pair.first) && pair.second == type)
		{
			++count;
		}
	}

	return count;
}

void BoardManager::InitializeJokerInfoMap()
{
	//------------------------------------------------------------------------------------------------ 일반 (set 1)
	m_jokerInfoMap[StoneAbility::jokerDouble] = { "jokerDouble.png" };
	m_jokerInfoMap[StoneAbility::jokerOmok] = { "jokerOmok.png" };
	m_jokerInfoMap[StoneAbility::jokerSamok] = { "jokerSamok.png" };
	m_jokerInfoMap[StoneAbility::jokerSammok] = { "jokerSammok.png" };

	//------------------------------------------------------------------------------------------------ 야생 (set 2)
	m_jokerInfoMap[StoneAbility::jokerEvolution] = { "jokerEvolution.png" };						// cost 0
	m_jokerInfoMap[StoneAbility::jokerDansu] = { "jokerDansu.png", 5, 5, 0 ,0 , 1};
	m_jokerInfoMap[StoneAbility::jokerEgg] = { "jokerEgg.png", 3, 0, 0, 2, 2 };
	m_jokerInfoMap[StoneAbility::jokerOstrichEgg] = { "jokerOstrichEgg.png", 2, 0, 0, 1, 3 };
	m_jokerInfoMap[StoneAbility::jokerPeacock] = { "jokerPeacock.png", 0, 0, 0, 3, 4 };				// cost 4

	//------------------------------------------------------------------------------------------------ 우주 (set 3)
	m_jokerInfoMap[StoneAbility::jokerTeleport] = { "jokerTeleport.png", 10, 5, 0 };
	m_jokerInfoMap[StoneAbility::jokerExplode] = { "jokerExplode.png", 0, 1, 0, 1 };
	m_jokerInfoMap[StoneAbility::jokerMagnetic] = { "jokerMagnetic.png", 0, 1, 0, 3 };
	m_jokerInfoMap[StoneAbility::jokerBlackhole] = { "jokerBlackhole.png", 0, 1, 0, 5 };

	//------------------------------------------------------------------------------------------------ 단청 (set 4)
	m_jokerInfoMap[StoneAbility::jokerFusion] = { "jokerFusion.png", 0, 0, 0, 2 };
	m_jokerInfoMap[StoneAbility::jokerTriunion] = { "jokerTriunion.png", 0, 0, 0, 3 };
	m_jokerInfoMap[StoneAbility::jokerQuadunion] = { "jokerQuadunion.png", 0, 0, 0, 4 };

	//------------------------------------------------------------------------------------------------ 할로윈 (set 6)
	m_jokerInfoMap[StoneAbility::jokerSplit] = { "jokerSplit.png", 3, 0, 0, 20 };
	m_jokerInfoMap[StoneAbility::jokerWaxseal] = { "jokerWaxseal.png", 0, 0, 0, 1 };
	m_jokerInfoMap[StoneAbility::jokerFlip] = { "jokerFlip.png", 20, 10, 0 };
	m_jokerInfoMap[StoneAbility::jokerOthello] = { "jokerOthello.png", 15, 7, 0 };
	m_jokerInfoMap[StoneAbility::jokerMrchan] = { "jokerMrchan.png", 20, 10, 0 };

	//------------------------------------------------------------------------------------------------ 자연 (set 7)
	m_jokerInfoMap[StoneAbility::jokerShadow] = { "jokerShadow.png", 10, 5, 0 };
	m_jokerInfoMap[StoneAbility::jokerLight] = { "jokerLight.png", 15, 7, 0 };
	m_jokerInfoMap[StoneAbility::jokerTime] = { "jokerTime.png", 20, 10, 0 };
	m_jokerInfoMap[StoneAbility::jokerWind] = { "jokerWind.png", 2, 5, 0, 2 };



// 	m_jokerInfoMap[StoneAbility::JokerEvolve] = { "JokerEvolve.png", 2, 7, 0 };
// 	m_jokerInfoMap[StoneAbility::JokerEgg] = { "JokerEgg.png", 0, 3, 0, 0, 1 };
// 	m_jokerInfoMap[StoneAbility::JokerOstrichEgg] = { "JokerOstrichEgg.png", 0, 2, 0, 0, 1 };
// 	m_jokerInfoMap[StoneAbility::JokerBite] = { "JokerBite.png", 30, 15, 0 };
// 	m_jokerInfoMap[StoneAbility::JokerPeacock] = { "JokerPeacock.png", 1, 0, 0, 0, 2 };
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
			if (!isValidPoint({nr, nc})) continue;

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
	for (POINT p : g)
	{
		m_board[p.x][p.y] = nullptr;
		m_stoneTypeMap.erase(p);

		if (IsJokerStone(p)) RemoveJokerStone(p); // 조커 돌 제거
	}
}

void BoardManager::CheckRemovedStones()
{
	for (int r = 0; r < SIZE_DEFAULT; ++r)
	{
		for (int c = 0; c < SIZE_DEFAULT; ++c)
		{
			if (m_board[r][c] && m_board[r][c]->m_isRemoved)
			{
				m_board[r][c] = nullptr;
				m_stoneTypeMap.erase({ r, c });
			}
		}
	}
}

void BoardManager::RemoveJokerStone(POINT position)
{
	m_jokerPositions.erase
	(
		remove_if
		(
			m_jokerPositions.begin(),
			m_jokerPositions.end(),
			[&](const pair<POINT, StoneAbility>& p) { return p.first == position; }
		),
		m_jokerPositions.end()
	);
}




//---------------------------------------------------------------- 희생 모드 관련 함수
bool BoardManager::SelectSacrificeStone(POINT mousePos)
{
	m_selectedPosition = MouseToBoardPosition(mousePos); // R,C변환

	if (!isValidPoint(m_selectedPosition))
	{
		std::cout << "unvaluable position : (" << m_selectedPosition.x << ", " << m_selectedPosition.y << ")" << std::endl;
		return false;
	}
	if (!m_board[m_selectedPosition.x][m_selectedPosition.y])
	{
		std::cout << "stone null : (" << m_selectedPosition.x << ", " << m_selectedPosition.y << ")" << std::endl;
		return false;
	}
	auto it = m_stoneTypeMap.find({ m_selectedPosition.x,m_selectedPosition.y });
	std::cout << m_selectedPosition.x << "  " << m_selectedPosition.y << std::endl;
	if (it == m_stoneTypeMap.end() || it->second != Black) return false; // 흑돌 아니면 패스
	selectGroup.push_back({ m_selectedPosition.x,m_selectedPosition.y });
	for (auto item : selectGroup) 
	{
		std::cout << item.x << item.y << std::endl;
	}
	
}

bool BoardManager::checkSelectsuccess()
{
	if (selectGroup.size() == m_jokerInfoMap[m_pendingAb].costBlack) 
	{
		RemoveGroup(selectGroup);
		return true;
	}
	return false;
}



//---------------------------------------------------------------- 버튼 - 상태 판정용 함수
int BoardManager::CountStones(StoneType t) const
{
	return GetStoneTypeAmount(t);          // 기존 함수 재사용
}

bool BoardManager::HasStraightLine(StoneType type, int len) const
{
	const int N = SIZE_DEFAULT;

	for (int r = 0; r < N; ++r)
		for (int c = 0; c < N; ++c)
		{
			auto it = m_stoneTypeMap.find({ r,c });
			if (it == m_stoneTypeMap.end() || it->second != type) continue;

			static const int dr[4] = { 1,0,1,1 };
			static const int dc[4] = { 0,1,1,-1 };
			for (int k = 0; k < 4; ++k)
			{
				int cnt = 1;
				int nr = r + dr[k], nc = c + dc[k];
				while (isValidPoint({ nr,nc }))
				{
					auto it2 = m_stoneTypeMap.find({ nr,nc });
					if (it2 == m_stoneTypeMap.end() || it2->second != type) break;
					if (++cnt >= len) return true;
					nr += dr[k];  nc += dc[k];
				}
			}
		}
	return false;
}

bool BoardManager::WhiteLibOne() const
{
	std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> visited{};   // 전체 보드 방문 체크

	for (const auto& kv : m_stoneTypeMap)              // 모든 흰돌 순회
	{
		if (kv.second != StoneType::White) continue;

		int r = kv.first.x;
		int c = kv.first.y;
		if (visited[r][c]) continue;                   // 이미 같은 그룹으로 처리됨

		std::vector<POINT> group;                      // 그룹 모임
		int libs = CountLiberty(r, c, group, visited); // 기존 함수 재사용

		if (libs == 1) return true;             // 자유도 1인 그룹 발견
	}
	return false;
}


bool BoardManager::IsColorCount(StoneType type, int need) const
{
	static const int DR8[8] = { -1,-1,-1, 0, 0, 1, 1, 1 };
	static const int DC8[8] = { -1, 0, 1,-1, 1,-1, 0, 1 };

	const int N = SIZE_DEFAULT;

	for (int r = 0; r < N; ++r)
		for (int c = 0; c < N; ++c)
		{

			if (m_board[r][c]) continue;                  

			int colorcount = 0;

			for (int k = 0; k < 8; ++k)
			{
				int nr = r + DR8[k], nc = c + DC8[k];
				if (!isValidPoint({ nr,nc })) continue;

				auto it = m_stoneTypeMap.find({ nr,nc });
				if (it != m_stoneTypeMap.end() &&
					it->second == type)
				{
					if (++colorcount >= need) return true;   // 조기 탈출
				}
			}
		}

	return false;
}


bool BoardManager::IsSamaBlackWhite() const
{
	const int N = SIZE_DEFAULT;

	for (int r = 0; r < N; ++r)
		for (int c = 0; c < N; ++c)
		{

			if (m_board[r][c]) continue;              

			int black = 0, white = 0;


			for (int dr = -1; dr <= 1; ++dr)
				for (int dc = -1; dc <= 1; ++dc)
				{
					if (dr == 0 && dc == 0) continue; 

					POINT p{ r + dr, c + dc };
					if (!isValidPoint(p)) continue;

					auto it = m_stoneTypeMap.find(p);
					if (it == m_stoneTypeMap.end()) continue;   

					if (it->second == StoneType::Black) ++black;
					else if (it->second == StoneType::White) ++white;
				}


			if (black > 0 && black == white)
				return true;
		}

	return false;
}

bool BoardManager::IsLibZero() const
{
	static const int DR8[8] = { -1,-1,-1, 0, 0, 1, 1, 1 };
	static const int DC8[8] = { -1,  0,  1,-1, 1,-1, 0, 1 };

	for (int row = 0; row < SIZE_DEFAULT; ++row)
		for (int col = 0; col < SIZE_DEFAULT; ++col)
		{
			if (m_board[row][col]) continue;

			int validNbr = 0;          
			int occNbr = 0;          

			for (int k = 0; k < 8; ++k)
			{
				int nr = row + DR8[k];
				int nc = col + DC8[k];
				if (!isValidPoint({ nc, nr }))      
					continue;                       

				++validNbr;
				if (m_board[nr][nc])               
					++occNbr;
			}

			if (validNbr > 0 && occNbr == validNbr)
				return true;
		}
	return false;
}

bool BoardManager::IsConnectTwo() const
{
	static const int DR[4] = { -1, 1,  0, 0 }; 
	static const int DC[4] = { 0, 0, -1, 1 };

	for (int r = 0; r < SIZE_DEFAULT; ++r)
		for (int c = 0; c < SIZE_DEFAULT; ++c)
		{
			if (m_board[r][c]) continue;

			int whiteCnt = 0;

			for (int i = 0; i < 4; ++i)
			{
				int nr = r + DR[i];
				int nc = c + DC[i];
				if (!isValidPoint({ nc, nr })) continue;   

				auto it = m_stoneTypeMap.find({ nr, nc });
				if (it != m_stoneTypeMap.end() &&
					it->second == StoneType::White)
				{
					++whiteCnt;
					if (whiteCnt > 2) break;            
				}
			}

			if (whiteCnt == 2)
				return true;
		}

	return false; 
}

bool BoardManager::IsOthello() const
{

	static const int DR[4] = { -1, 1,  0, 0 };
	static const int DC[4] = { 0, 0, -1, 1 };

	for (const auto& kv : m_stoneTypeMap)         
	{
		if (kv.second != StoneType::Black) continue;

		int r0 = kv.first.x;    
		int c0 = kv.first.y; // 흑돌 찾기

		for (int dir = 0; dir < 4; ++dir) // 방향 설정
		{
			int r = r0 + DR[dir];
			int c = c0 + DC[dir];


			auto it = m_stoneTypeMap.find({ r, c });
			if (it == m_stoneTypeMap.end() || it->second != StoneType::White) // 백돌 찾기
				continue;             


			int cnt = 0;
			while (true)
			{
				if (!isValidPoint({ c, r })) break;     

				auto it2 = m_stoneTypeMap.find({ r, c });
				if (it2 == m_stoneTypeMap.end() ||
					it2->second != StoneType::White)
					break;                                  // 백돌이 끊김

				++cnt;
				r += DR[dir];
				c += DC[dir];
			}

			bool enough = (cnt >= 3);
			bool nextCellEmpty =
				isValidPoint({ c, r }) &&    
				m_stoneTypeMap.find({ r, c }) == m_stoneTypeMap.end();

			if (enough && nextCellEmpty)
				return true;   
		}
	}
	return false; 
}

