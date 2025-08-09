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
		{ StoneAbility::jokerSammok, [this](shared_ptr<JokerStone> jokerSammok, POINT position)
		{
			POINT direction[4] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }; // 상하좌우

			POINT newPosition = position;

			for (POINT i : direction)
			{
				for (int j = 1; j < SIZE_DEFAULT; ++j)
				{
					newPosition = { position.x + i.x * j, position.y + i.y * j };
					if (!boardManager.isValidPoint(newPosition)) break;

					if (boardManager.m_board[newPosition.x][newPosition.y])
					{
						if (boardManager.IsJokerStone(newPosition)) continue; // 조커 돌은 무시

						if (boardManager.m_stoneTypeMap.find(newPosition)->second == StoneType::White)
						{
							boardManager.m_playerInfo.m_money++;
							boardManager.RemoveGroup({ newPosition });
						}
					}
				}
			}
			std::cout << "jokerSammok check" << std::endl;
		}
		},
		{ StoneAbility::jokerOmok, [this](shared_ptr<JokerStone> jokerOmok, POINT position)
		{
			if (!jokerOmok->m_jokerInfo.coolTime) return;
			jokerOmok->m_jokerInfo.coolTime--;

			POINT directionX[4] = { { -1, 0 }, { 1, 0 }, { -2, 0 }, { 2, 0 } }; // 좌우
			POINT directionY[4] = { { 0, -1 }, { 0, 1 }, { 0, -2 }, { 0, 2 } }; // 상하

			if (!boardManager.m_isVertical)
			{
				for (POINT i : directionX)
				{
					POINT newPosition = { position.x + i.x, position.y + i.y };
					if (!boardManager.isValidPoint(newPosition)) continue;
					if (boardManager.m_board[newPosition.x][newPosition.y]) continue;

					boardManager.PlaceStone(newPosition, StoneType::Black, StoneAbility::None); // 검은 돌 놓기
					boardManager.WhiteStoneRemoveCheck(newPosition);
					boardManager.m_playerInfo.incBlackCount(1);
				}
			}
			else
			{
				for (POINT i : directionY)
				{
					POINT newPosition = { position.x + i.x, position.y + i.y };
					if (!boardManager.isValidPoint(newPosition)) continue;
					if (boardManager.m_board[newPosition.x][newPosition.y]) continue;

					boardManager.PlaceStone(newPosition, StoneType::Black, StoneAbility::None); // 검은 돌 놓기
					boardManager.WhiteStoneRemoveCheck(newPosition);
					boardManager.m_playerInfo.incBlackCount(1);
				}
			}
		}
		},
		{ StoneAbility::jokerEgg, [this](shared_ptr<JokerStone> jokerEgg, POINT position)
		{
			if (!jokerEgg->m_jokerInfo.coolTime)
			{
				boardManager.m_playerInfo.incBlackCount(jokerEgg->m_jokerInfo.functionVariable);
				jokerEgg->m_jokerInfo.coolTime = m_jokerInfoMap.find(StoneAbility::jokerEgg)->second.coolTime; // 쿨타임 초기화
			}
			jokerEgg->m_jokerInfo.coolTime--;

		}
		},
		{ StoneAbility::jokerOstrichEgg, [this](shared_ptr<JokerStone> jokerOstrichEgg, POINT position)
		{
			if (!jokerOstrichEgg->m_jokerInfo.coolTime)
			{
				boardManager.m_playerInfo.incBlackCount(jokerOstrichEgg->m_jokerInfo.functionVariable);
				jokerOstrichEgg->m_jokerInfo.coolTime = m_jokerInfoMap.find(StoneAbility::jokerOstrichEgg)->second.coolTime; // 쿨타임 초기화
			}
			jokerOstrichEgg->m_jokerInfo.coolTime--;

			std::cout << boardManager.m_playerInfo.m_BlackStone << std::endl;
		}
		},
		{ StoneAbility::jokerDansu, [this](shared_ptr<JokerStone> jokerBite, POINT position)
		{
			if (!jokerBite->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			jokerBite->m_jokerInfo.lifeSpan--;
			if (!jokerBite->m_jokerInfo.coolTime) return;
			jokerBite->m_jokerInfo.coolTime--;

			POINT direction[4] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }; // 상하좌우
			int directionInt = 0;
			int maxWhiteStone = 0;

			POINT newPosition = position;

			for (int i = 0; i < 4; ++i)
			{
				int tempWhiteStone = 0;
				for (int j = 1; j < SIZE_DEFAULT; ++j)
				{
					newPosition = { position.x + direction[i].x * j, position.y + direction[i].y * j };
					if (!boardManager.isValidPoint(newPosition)) break;

					if (boardManager.m_board[newPosition.x][newPosition.y])
					{
						if (boardManager.IsJokerStone(newPosition)) continue; // 조커 돌은 무시

						if (boardManager.m_stoneTypeMap.find(newPosition)->second == StoneType::White) tempWhiteStone++;
					}
				}
				if (tempWhiteStone > maxWhiteStone)
				{
					maxWhiteStone = tempWhiteStone;
					directionInt = i;
				}
			}
			for (int i = 1; i < SIZE_DEFAULT; ++i)
			{
				newPosition = { position.x + direction[directionInt].x * i, position.y + direction[directionInt].y * i };
				if (!boardManager.isValidPoint(newPosition)) break;

				if (boardManager.m_board[newPosition.x][newPosition.y])
				{
					if (boardManager.IsJokerStone(newPosition)) continue; // 조커 돌은 무시

					if (boardManager.m_stoneTypeMap.find(newPosition)->second == StoneType::Black)
					{
						//boardManager.m_playerInfo.m_BlackStone++; // 증가 안시켜도됨
					}
					else if (boardManager.m_stoneTypeMap.find(newPosition)->second == StoneType::White)
					{
						boardManager.m_playerInfo.m_money++;
					}
					boardManager.RemoveGroup({ newPosition });
				}
			}

			jokerBite->Move(boardManager.BoardToScreenPosition(newPosition), 1);
		}
		},
		{ StoneAbility::jokerPeacock, [this](shared_ptr<JokerStone> jokerPeacock, POINT position)
		{
			if (!jokerPeacock->m_jokerInfo.coolTime) return;
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

		{ StoneAbility::jokerTeleport, [this](shared_ptr<JokerStone> jokerTeleport, POINT position)
		{
			if (!jokerTeleport->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			if (!jokerTeleport->m_jokerInfo.coolTime) return;
			jokerTeleport->m_jokerInfo.lifeSpan--;
			jokerTeleport->m_jokerInfo.coolTime--;

			boardManager.RemoveGroup(boardManager.m_useCondGroup);
			jokerTeleport->Move(boardManager.BoardToScreenPosition(boardManager.m_useCondGroup.front()), 0.001);
			for (auto& pos : boardManager.m_jokerPositions)
			{
				if (pos.first == position)
				{
					pos.first = boardManager.m_useCondGroup.front();
					break;
				}
			}
			boardManager.m_stoneTypeMap[boardManager.m_useCondGroup.front()] = boardManager.m_stoneTypeMap[position];
			boardManager.m_board[boardManager.m_useCondGroup.front().x][boardManager.m_useCondGroup.front().y] = boardManager.m_board[position.x][position.y];
			boardManager.m_board[position.x][position.y] = nullptr;
			boardManager.PlaceStone(position, StoneType::White, StoneAbility::None);
			boardManager.WhiteStoneRemoveCheck({ position.x - 1, position.y });
		}
		},
		{ StoneAbility::jokerExplode, [this](shared_ptr<JokerStone> jokerExplode, POINT position)
		{
			if (!jokerExplode->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			if (!jokerExplode->m_jokerInfo.coolTime) return;

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
						//boardManager.m_playerInfo.m_BlackStone++; //없어도됨
					}

					boardManager.RemoveGroup({ { x, y } });
				}
			}
			jokerExplode->m_jokerInfo.lifeSpan--;
			jokerExplode->m_jokerInfo.coolTime--;
		}
		},
		{ StoneAbility::jokerMagnetic, [this](shared_ptr<JokerStone> jokerMagnetic, POINT position)
		{
			if (!jokerMagnetic->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			if (!jokerMagnetic->m_jokerInfo.coolTime) return;
			jokerMagnetic->m_jokerInfo.coolTime--;
			jokerMagnetic->m_jokerInfo.lifeSpan--;

			int patternX[8] = { -1, 0, 1, 1, -1, -1, 0, 1 };
			int patternY[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

			for (int j = 0; j < 8; ++j)
			{
				for (int i = 1; i < jokerMagnetic->m_jokerInfo.functionVariable; ++i)
				{
					int newX = position.x + patternX[j] * i;
					int newY = position.y + patternY[j] * i;

					if (!boardManager.isValidPoint({ newX, newY })) break;
					if (!boardManager.m_board[newX][newY]) break;

					random_device rd;
					mt19937 rng(rd());
					uniform_int_distribution<int> dist(0, 1);
					int randomValue = dist(rng);

					if (randomValue)
					{
						if (boardManager.isValidPoint({ newX + patternX[j], newY + patternY[j] }) && !boardManager.m_board[newX + patternX[j]][newY + patternY[j]])
						{
							boardManager.m_board[newX][newY]->Move(boardManager.BoardToScreenPosition({ newX + patternX[j], newY + patternY[j] }), 1);
							boardManager.m_board[newX + patternX[j]][newY + patternY[j]] = boardManager.m_board[newX][newY];
							boardManager.m_board[newX][newY] = nullptr;
							boardManager.m_stoneTypeMap[{ newX + patternX[j], newY + patternY[j] }] = boardManager.m_stoneTypeMap[{ newX, newY }];
							boardManager.m_stoneTypeMap.erase({ newX, newY });
							if (boardManager.IsJokerStone({ newX, newY }))
							{
								for (auto& pair : boardManager.m_jokerPositions)
								{
									if (pair.first == POINT{ newX, newY }) pair.first = { newX + patternX[j], newY + patternY[j] };
								}
							}
							break;
						}
					}
					else
					{
						if (boardManager.isValidPoint({ newX - patternX[j], newY - patternY[j] }) && !boardManager.m_board[newX - patternX[j]][newY - patternY[j]])
						{
							boardManager.m_board[newX][newY]->Move(boardManager.BoardToScreenPosition({ newX - patternX[j], newY - patternY[j] }), 1);
							boardManager.m_board[newX - patternX[j]][newY - patternY[j]] = boardManager.m_board[newX][newY];
							boardManager.m_board[newX][newY] = nullptr;
							boardManager.m_stoneTypeMap[{ newX - patternX[j], newY - patternY[j] }] = boardManager.m_stoneTypeMap[{ newX, newY }];
							boardManager.m_stoneTypeMap.erase({ newX, newY });
							if (boardManager.IsJokerStone({ newX, newY }))
							{
								for (auto& pair : boardManager.m_jokerPositions)
								{
									if (pair.first == POINT{ newX, newY }) pair.first = { newX - patternX[j], newY - patternY[j] };
								}
							}
							break;
						}
					}
				}
			}
		}
		},
		{ StoneAbility::jokerBlackhole, [this](shared_ptr<JokerStone> jokerBlackhole, POINT position)
		{
			// Mediator에 위치 설정
			if (!jokerBlackhole->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			if (!jokerBlackhole->m_jokerInfo.coolTime) return;

			int functionVariable = jokerBlackhole->m_jokerInfo.functionVariable;
			for (const auto& pair : boardManager.GetStoneTypeMap())
			{
				if (abs(pair.first.x - position.x) <= functionVariable && abs(pair.first.y - position.y) <= functionVariable && (abs(pair.first.x - position.x) > 0 || abs(pair.first.y - position.y) > 0))
				{
					Mediator::GetInstance().SetPosition(jokerBlackhole->GetPosition());
					SceneManager::GetInstance().ChangePostProcessing("BlackHole"); // 필터 적용
				
					boardManager.m_board[pair.first.x][pair.first.y]->Move(boardManager.BoardToScreenPosition({ position.x, position.y }), 3);
					boardManager.m_board[pair.first.x][pair.first.y]->Remove(3);

					if (boardManager.m_stoneTypeMap.find(pair.first)->second == StoneType::Black)
					{
						//boardManager.m_playerInfo.m_BlackStone++;
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
				boardManager.m_playerInfo.m_money += jokerFusion->m_jokerInfo.functionVariable * grp.size();
			}


		}
		},

		{ StoneAbility::jokerTriunion, [this](shared_ptr<JokerStone> jokerTriunion, POINT position)
		{
			vector<POINT> grp;
			array<array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> vis{};
			if (boardManager.CountLiberty(position.x, position.y, grp, vis) == 0)
			{
				boardManager.m_playerInfo.m_money += jokerTriunion->m_jokerInfo.functionVariable * grp.size();
			}


		}
		},
		{ StoneAbility::jokerQuadunion, [this](shared_ptr<JokerStone> jokerQuadunion, POINT position)
		{
			vector<POINT> grp;
			array<array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> vis{};
			if (boardManager.CountLiberty(position.x, position.y, grp, vis) == 0)
			{
				boardManager.m_playerInfo.m_money += jokerQuadunion->m_jokerInfo.functionVariable * grp.size();
				boardManager.RemoveJokerStone(position);
			}

		}
		},


		{ StoneAbility::jokerSplit, [this](shared_ptr<JokerStone> jokerSplit, POINT position)
		{
			if (!jokerSplit->m_jokerInfo.coolTime)
			{
				random_device rd;
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
			// 일단은 흰돌도 줌

			std::cout << "Wax Money : " << boardManager.m_playerInfo.m_waxMoney << std::endl;
		}
		},
		{ StoneAbility::jokerFlip, [this](shared_ptr<JokerStone> jokerFlip, POINT position)
		{
			if (!jokerFlip->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			if (!jokerFlip->m_jokerInfo.coolTime) return;
			jokerFlip->m_jokerInfo.lifeSpan--;
			jokerFlip->m_jokerInfo.coolTime--;

			POINT patternX[9] = { { -1, 0 }, { -2, 0 }, { -2, -1 }, { -2, 1 }, { -3, 0 }, { -3, -1 }, { -3, 1 }, { -3, -2 }, { -3, 2 } };
			POINT patternY[9] = { { 0, -1 }, { 0, -2 }, { -1, -2 }, { 1, -2 }, { 0, -3 }, { -1, -3 }, { 1, -3 }, { -2, -3 }, { 2, -3 } };
			int inverse[2] = { 1, -1 };

			bool isX = true;
			int directionInt = 0;
			int maxWhiteStone = 0;

			POINT newPosition = position;

			for (int dir : inverse)
			{
				int tempStone = 0;
				for (POINT pattern : patternX)
				{
					newPosition = { position.x + pattern.x * dir, position.y + pattern.y * dir };
					if (!boardManager.isValidPoint(newPosition)) continue;

					if (boardManager.m_board[newPosition.x][newPosition.y])
					{
						if (!boardManager.IsJokerStone(newPosition)) tempStone++;
					}
				}
				if (tempStone > maxWhiteStone)
				{
					maxWhiteStone = tempStone;
					isX = true;
					directionInt = dir;
				}
			}
			for (int dir : inverse)
			{
				int tempStone = 0;
				for (POINT pattern : patternY)
				{
					newPosition = { position.x + pattern.x * dir, position.y + pattern.y * dir };
					if (!boardManager.isValidPoint(newPosition)) continue;

					if (boardManager.m_board[newPosition.x][newPosition.y])
					{
						if (!boardManager.IsJokerStone(newPosition)) tempStone++;
					}
				}
				if (tempStone > maxWhiteStone)
				{
					maxWhiteStone = tempStone;
					isX = false;
					directionInt = dir;
				}
			}
			if (isX)
			{
				for (POINT pattern : patternX)
				{
					newPosition = { position.x + pattern.x * directionInt, position.y + pattern.y * directionInt };
					if (!boardManager.isValidPoint(newPosition)) continue;

					if (boardManager.m_board[newPosition.x][newPosition.y])
					{
						if (boardManager.IsJokerStone(newPosition)) continue;
						if (boardManager.m_stoneTypeMap.find(newPosition)->second == StoneType::White)
						{
							boardManager.m_board[newPosition.x][newPosition.y]->ChangeColor();
						}
						else if (boardManager.m_stoneTypeMap.find(newPosition)->second == StoneType::Black)
						{
							boardManager.m_board[newPosition.x][newPosition.y]->ChangeColor(false);
						}
					}
				}
			}
			else
			{
				for (POINT pattern : patternY)
				{
					newPosition = { position.x + pattern.x * directionInt, position.y + pattern.y * directionInt };
					if (!boardManager.isValidPoint(newPosition)) continue;

					if (boardManager.m_board[newPosition.x][newPosition.y])
					{
						if (boardManager.IsJokerStone(newPosition)) continue;
						if (boardManager.m_stoneTypeMap.find(newPosition)->second == StoneType::White)
						{
							boardManager.m_board[newPosition.x][newPosition.y]->ChangeColor();
						}
						else if (boardManager.m_stoneTypeMap.find(newPosition)->second == StoneType::Black)
						{
							boardManager.m_board[newPosition.x][newPosition.y]->ChangeColor(false);
						}
					}
				}
			}
		}
		},
		{ StoneAbility::jokerOthello, [this](shared_ptr<JokerStone> jokerOthello, POINT position)
		{
			if (!jokerOthello->m_jokerInfo.coolTime) return;
			jokerOthello->m_jokerInfo.coolTime--;

			int patternX[4] = { -1, 1, 0, 0 };
			int patternY[4] = { 0, 0, -1, 1 };

			for (int i = 0; i < 4; ++i)
			{
				int x = position.x + patternX[i];
				int y = position.y + patternY[i];

				int count = 0;
				vector<POINT> group;

				for (int j = 0; j < SIZE_DEFAULT; ++j)
				{
					int nx = x + patternX[i] * j;
					int ny = y + patternY[i] * j;

					if (!boardManager.isValidPoint({ nx, ny }) || !boardManager.m_board[nx][ny]) { count = 0; break; }
					if (boardManager.m_stoneTypeMap.find({ nx, ny })->second == StoneType::White) { count++; group.push_back({ nx, ny }); }
					if (boardManager.m_stoneTypeMap.find({ nx, ny })->second == StoneType::Black) break;
				}
				if (count >= jokerOthello->m_jokerInfo.functionVariable)
				{
					boardManager.m_playerInfo.m_money += count;

					for (const auto& pos : group)
					{
						boardManager.m_board[pos.x][pos.y]->ChangeColor();
						boardManager.m_stoneTypeMap[pos] = StoneType::Black;
					}
				}
			}
		}
		},
		{ StoneAbility::jokerMrchan, [this](shared_ptr<JokerStone> jokerMrchan, POINT position)
		{
			if (!jokerMrchan->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			if (!jokerMrchan->m_jokerInfo.coolTime) return;
			jokerMrchan->m_jokerInfo.lifeSpan--;
			jokerMrchan->m_jokerInfo.coolTime--;

			vector<POINT> whiteStones;
			for (auto& pair : boardManager.m_stoneTypeMap)
			{
				if (boardManager.IsJokerStone(pair.first)) continue;
				if (pair.second == StoneType::White)
				{
					whiteStones.push_back(pair.first);
				}
			}

			random_device rd;
			mt19937 rng(rd());
			shuffle(whiteStones.begin(), whiteStones.end(), rng);

			uniform_int_distribution<int> dist(1, 15);
			int randomIndex = dist(rng);
			for (int i = 0; i < randomIndex && i < whiteStones.size(); ++i)
			{
				if (boardManager.m_board[whiteStones[i].x][whiteStones[i].y])
				{
					boardManager.RemoveGroup({ whiteStones[i] });
				}
			}
		}
		},

		{ StoneAbility::jokerShadow, [this](shared_ptr<JokerStone> jokerShadow, POINT position)
		{
			if (!jokerShadow->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			if (!jokerShadow->m_jokerInfo.coolTime) return;
			jokerShadow->m_jokerInfo.coolTime--;
			jokerShadow->m_jokerInfo.lifeSpan--;
			int patternXY[2] = { -1, 1 };
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					if (boardManager.isValidPoint({ position.x + patternXY[i], position.y + patternXY[j] }) && boardManager.m_board[position.x + patternXY[i]][position.y + patternXY[j]])
					{
						if (boardManager.m_stoneTypeMap.find({ position.x + patternXY[i], position.y + patternXY[j] })->second == StoneType::White)
						{

							boardManager.m_playerInfo.incBlackCount(jokerShadow->m_jokerInfo.functionVariable);
							std::cout << "Black Stone : " << boardManager.m_playerInfo.GetBlackCount() << std::endl;

							return;
						}
					}
				}
			}

		}
		},
		{ StoneAbility::jokerLight, [this](shared_ptr<JokerStone> jokerLight, POINT position)
		{
			if (!jokerLight->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});
			if (!jokerLight->m_jokerInfo.coolTime) return;
			jokerLight->m_jokerInfo.coolTime--;
			jokerLight->m_jokerInfo.lifeSpan--;

			int patternX[8] = { -1, 0, 1, 1, -1, -1, 0, 1 };
			int patternY[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

			for (int i = 0; i < SIZE_DEFAULT; ++i)
			{
				for (int j = 0; j < 8; ++j)
				{
					int newX = position.x + patternX[j] * i;
					int newY = position.y + patternY[j] * i;
					if (!boardManager.isValidPoint({ newX, newY })) continue;
					if (boardManager.m_board[newX][newY])
					{
						if (boardManager.IsJokerStone({ newX, newY })) continue;
						if (boardManager.m_stoneTypeMap.find({ newX, newY })->second == StoneType::White)
						{
							if (boardManager.isValidPoint({ newX + patternX[j], newY + patternY[j] }) && !boardManager.m_board[newX + patternX[j]][newY + patternY[j]])
							{
								boardManager.PlaceStone({ newX + patternX[j], newY + patternY[j] }, StoneType::Black, StoneAbility::None);
								boardManager.WhiteStoneRemoveCheck(position);
							}
						}
					}
				}
			}
		}
		},
		{ StoneAbility::jokerTime, [this](shared_ptr<JokerStone> jokerTime, POINT position)
		{
			if (!jokerTime->m_jokerInfo.coolTime)
			{
				std::random_device rd;
				mt19937 rng(rd());
				uniform_int_distribution<int> distR(jokerTime->m_jokerInfo.functionVariable, 15);
				uniform_int_distribution<int> distC(m_jokerInfoMap.find(StoneAbility::jokerTime)->second.coolTime, 5);

				int tempR = distR(rng);
				boardManager.m_playerInfo.incBlackCount(tempR);
				std::cout << "Black Stone : " << boardManager.m_playerInfo.GetBlackCount() << std::endl;

				int tempC = distC(rng);
				jokerTime->m_jokerInfo.coolTime = tempC;
			}
			jokerTime->m_jokerInfo.coolTime--;
		}
		},
		{ StoneAbility::jokerWind, [this](shared_ptr<JokerStone> jokerWind, POINT position)
		{
			if (!jokerWind->m_jokerInfo.lifeSpan) boardManager.RemoveGroup({position});

			if (!jokerWind->m_jokerInfo.coolTime)
			{
				random_device rd;
				mt19937 rng(rd());
				uniform_int_distribution<int> dist(0, 3);
				int randomDirectionIndex = dist(rng);

				POINT randomDirection = { position.x + DR[randomDirectionIndex], position.y + DC[randomDirectionIndex] };
				if (!boardManager.isValidPoint(randomDirection) || !boardManager.m_board[randomDirection.x][randomDirection.y]) return;
				
				auto it = std::find_if(boardManager.m_jokerPositions.begin(), boardManager.m_jokerPositions.end(),
					[&](const std::pair<POINT, StoneAbility>& p) { return p.first == randomDirection; });
				if (it != boardManager.m_jokerPositions.end()) return;

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

	CheckRemovedStones();
	if (!PlaceStone(m_selectedPosition, m_stoneType, m_stoneAbility)) return false;
	m_playerInfo.decBlackCount(m_SacrificeGroup.size());
	m_playerInfo.incBlackCount(m_jokerInfoMap[m_stoneAbility].returnBlack);
	RemoveGroup(m_SacrificeGroup);
	JokerAbilityUpdate(); // 조커 능력 업데이트
	WhiteStoneRemoveCheck(m_selectedPosition); // 흰 돌 체크

	m_selectedPosition = { -1, -1 }; // 마지막으로 선택된 위치 초기화

	m_stoneType = StoneType::Black; // 돌 타입 초기화

	m_stoneAbility = StoneAbility::None; // 돌 능력 초기화
	

	return true;
}

bool BoardManager::InputBasedGameLoop(int row, int col) // 바둑판 기준 row , col 입력 받아서 해당 배열에 액세스 해서 넣으면댐
{
	CheckRemovedStones();
	if (!PlaceStone({ row,col }, m_stoneType, m_stoneAbility)) return false;
	m_playerInfo.decBlackCount(m_SacrificeGroup.size());
	m_playerInfo.incBlackCount(m_jokerInfoMap[m_stoneAbility].returnBlack);
	RemoveGroup(m_SacrificeGroup);
	JokerAbilityUpdate(); // 조커 능력 업데이트
	WhiteStoneRemoveCheck(m_selectedPosition); // 흰 돌 체크

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
			std::cout <<"joker : " << jokerStone->GetAbility() << std::endl;
		}
	}
}

void BoardManager::JokerAbilityUse(StoneAbility ab, POINT position)
{
	JokerFunctionsWrapper wrapper;
	shared_ptr<Stone> stone = GetStone(position); // 해당 위치의 Stone(Object) 가져오기
	shared_ptr<JokerStone> jokerStone = dynamic_pointer_cast<JokerStone>(stone); // Stone을 JokerStone으로 캐스팅
	auto it = wrapper.g_abilityFunctions.find(ab);
	if (it != wrapper.g_abilityFunctions.end())
	{
		it->second(jokerStone, position); // 조커 능력 함수 실행
	}

}

POINT BoardManager::MouseToBoardPosition(POINT mousePos) const
{
	return
	{
		(mousePos.x + m_drawW + m_cell / 2) / m_cell,
		-(mousePos.y - m_drawH - m_cell / 2) / m_cell

	};
}


POINT BoardManager::BoardToScreenPosition(POINT boardPos) const
{
	return
	{
		boardPos.x * m_cell- m_drawW - m_cell / 2,
		-boardPos.y * m_cell + m_drawH - m_cell / 2

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
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<JokerStone>(BoardToScreenPosition(selectedPosition), m_cell, m_stoneOffset, stoneAbility, m_jokerInfoMap[stoneAbility].jokerType);

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
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<JokerStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset, stoneAbility, m_jokerInfoMap[stoneAbility].jokerType);

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
	m_deathRow.clear();
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
	m_jokerInfoMap[StoneAbility::None] = {}; // 디폴트 돌

	//------------------------------------------------------------------------------------------------ 일반 (set 1)
	m_jokerInfoMap[StoneAbility::jokerOmok]			= { "jokerOmok.png", "T_JokerOmok_Tooltip.png",				JokerType::Default, 1, 0, 0, 0, 5, 2, 4, 2, true, StoneType::Black };
	m_jokerInfoMap[StoneAbility::jokerSamok]		= { "jokerSamok.png", "T_JokerSamok_Tooltip.png",			JokerType::Default, 0, 0, 0, 0, 0, 0, 3, 2, false };
	m_jokerInfoMap[StoneAbility::jokerSammok]		= { "jokerSammok.png", "T_JokerSammok_Tooltip.png",			JokerType::Default, 0, 0, 0, 0, 0, 2, 2, 1, false };

	//------------------------------------------------------------------------------------------------ 야생 (set 2)
	m_jokerInfoMap[StoneAbility::jokerEvolution]	= { "jokerEvolution.png", "T_JokerEvolution_Tooltip.png",	JokerType::Wild, 0, 0, 0, 0, 1, 0, 7, 3, false };						// cost 0
	m_jokerInfoMap[StoneAbility::jokerDansu]		= { "jokerDansu.png", "T_JokerDansu_Tooltip.png",			JokerType::Wild, 1, 1, 0, 0, 2, 0, 2, 1, true, StoneType::Black };
	m_jokerInfoMap[StoneAbility::jokerEgg]			= { "jokerEgg.png",	"T_JokerEgg_Tooltip.png",				JokerType::Wild, 3, 0, 0, 1, 5, 0, 2, 1, true };
	m_jokerInfoMap[StoneAbility::jokerOstrichEgg]	= { "jokerOstrichEgg.png", "T_JokerEgg_Tooltip.png",		JokerType::Wild, 2, 0, 0, 1, 0, 0, 0, 2, true };
	m_jokerInfoMap[StoneAbility::jokerPeacock]		= { "jokerPeacock.png",	"T_JokerPeacock_Tooltip.png",		JokerType::Wild, 1, 1, 0, 3, 3, 1, 6, 3, true, StoneType::Black };				// cost 4

	//------------------------------------------------------------------------------------------------ 우주 (set 3)
	m_jokerInfoMap[StoneAbility::jokerTeleport]		= { "jokerTeleport.png", "T_JokerTeleport_Tooltip.png",		JokerType::Space, 1, 1, 0, 0, 1, 0, 4, 2, true };
	m_jokerInfoMap[StoneAbility::jokerExplode]		= { "jokerExplode.png",	"T_JokerExplode_Tooltip.png",		JokerType::Space, 1, 1, 0, 1, 3, 0, 6, 3, true };
	m_jokerInfoMap[StoneAbility::jokerMagnetic]		= { "jokerMagnetic.png", "T_JokerMagnetic_Tooltip.png",		JokerType::Space, 1, 1, 0, 3, 4, 2, 5, 2, true };
	m_jokerInfoMap[StoneAbility::jokerBlackhole]	= { "jokerBlackhole.png", "T_JokerBlackhole_Tooltip.png",	JokerType::Space, 1, 1, 0, 5, 15, 0, 8, 3, true };

	//------------------------------------------------------------------------------------------------ 단청 (set 4)
	m_jokerInfoMap[StoneAbility::jokerFusion]		= { "jokerFusion.png", "T_JokerFusion_Tooltip.png",			JokerType::Dancheong, 0, 0, 0, 2, 2, 0, 5, 2, true, StoneType::White };
	m_jokerInfoMap[StoneAbility::jokerTriunion]		= { "jokerTriunion.png", "T_JokerFusion_Tooltip.png",		JokerType::Dancheong, 0, 0, 0, 3, 0, 0, 3, 2, true, StoneType::White };
	m_jokerInfoMap[StoneAbility::jokerQuadunion]	= { "jokerQuadunion.png", "T_JokerFusion_Tooltip.png",		JokerType::Dancheong, 0, 0, 0, 4, 0, 0, 4, 3, true, StoneType::White };

	//------------------------------------------------------------------------------------------------ 할로윈 (set 6)
	m_jokerInfoMap[StoneAbility::jokerSplit]		= { "jokerSplit.png", "T_JokerSplit_Tooltip.png",			JokerType::Halloween, 3, 0, 0, 20, 2, 0, 5, 2, true };
	m_jokerInfoMap[StoneAbility::jokerWaxseal]		= { "jokerWaxseal.png",	"T_JokerWaxseal_Tooltip.png",		JokerType::Halloween, 0, 0, 0, 1, 0, 0, 3, 2, true, StoneType::Black };
	m_jokerInfoMap[StoneAbility::jokerFlip]			= { "jokerFlip.png", "T_JokerFlip_Tooltip.png",				JokerType::Halloween, 1, 1, 0, 0, 3, 1, 6, 2, true };
	m_jokerInfoMap[StoneAbility::jokerOthello]		= { "jokerOthello.png", "T_JokerOthello_Tooltip.png",		JokerType::Halloween, 1, 0, 0, 3, 2, 4, 5, 1, true, StoneType::Black };
	m_jokerInfoMap[StoneAbility::jokerMrchan]		= { "jokerMrchan.png", "T_JokerMrchan_Tooltip.png",			JokerType::Halloween, 1, 1, 0, 0, 0, 0, 0, 3, true };

	//------------------------------------------------------------------------------------------------ 자연 (set 7)
	m_jokerInfoMap[StoneAbility::jokerShadow]		= { "jokerShadow.png", "T_JokerShadow_Tooltip.png",			JokerType::Natural, 1, 1, 0, 4, 1, 0, 4, 2, true };
	m_jokerInfoMap[StoneAbility::jokerLight]		= { "jokerLight.png", "T_JokerLight_Tooltip.png",			JokerType::Natural, 1, 1, 0, 0, 0, 0, 4, 2, true };
	m_jokerInfoMap[StoneAbility::jokerTime]			= { "jokerTime.png", "T_JokerTime_Tooltip.png",				JokerType::Natural, 3, 0, 0, 2, 0, 2, 7, 3, true };
	m_jokerInfoMap[StoneAbility::jokerWind]			= { "jokerWind.png", "T_JokerWind_Tooltip.png",				JokerType::Natural, 2, 5, 0, 2, 4, 1, 4, 2, true };
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
		m_board[p.x][p.y]->DeathMove(1);
		m_deathRow.push_back(move(m_board[p.x][p.y])); // 죽은 돌을 deathRow에 추가
		m_board[p.x][p.y] = nullptr;

		if (m_stoneTypeMap.find(p) != m_stoneTypeMap.end()) m_stoneTypeMap.erase(p);
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
				m_board[r][c]->m_isRemoved = false; // 돌 제거 상태 초기화
				m_board[r][c]->DeathMove(1);
				m_deathRow.push_back(move(m_board[r][c])); // 죽은 돌을 deathRow에 추가
				m_board[r][c] = nullptr;

				if (m_stoneTypeMap.find({ r, c }) != m_stoneTypeMap.end())
				{
					if (IsJokerStone({ r, c })) RemoveJokerStone({ r, c }); // 조커 돌 제거
					m_stoneTypeMap.erase({ r, c });
				}
			}
		}
	}
	for (shared_ptr<Stone>& stone : m_deathRow) 
	{
		if (stone && stone->m_isRemoved)
		{
			stone = nullptr; // 죽은 돌을 nullptr로 설정
			m_deathRow.erase(remove(m_deathRow.begin(), m_deathRow.end(), stone), m_deathRow.end());
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



//---------------------------------------------------------------- 착수 영역 표시 및 제한
void BoardManager::ComputePlacementHints(StoneAbility ability)
{
	if (ability == StoneAbility::None) return;
	m_hintCells.clear();
	m_mask.clear();

	for (int x = 0; x < SIZE_DEFAULT; ++x)
		for (int y = 0; y < SIZE_DEFAULT; ++y)
			if (IsEmpty(x, y)) {
				bool ok = false;
				switch (ability) {
				case StoneAbility::jokerOmok:
				{
					if (m_stoneTypeMap.find({ x,y }) != m_stoneTypeMap.end()) return;
					ok = true;

					if (m_isVertical) { // 수직인 경우
						for (int dy = -2; dy <= 2; ++dy) {
							if (dy == 0) continue;
							int ny = y + dy;    int nx = x;
							if (!isValidPoint({ nx, ny }) || !IsEmpty(nx, ny)) {
								ok = false;
								break;
							}
						}
					}
					else {  // 수평인 경우
						for (int dx = -2; dx <= 2; ++dx) {
							if (dx == 0) continue;
							int nx =x + dx;
							int ny = y;
							if (!isValidPoint({ nx, ny }) || !IsEmpty(nx, ny)) {
								ok = false;
								break;
							}
						}
					}
				}
				break;

				case StoneAbility::jokerFusion:
				{
					if (m_stoneTypeMap.find({ x,y }) != m_stoneTypeMap.end()) return;
					ok = false;
					int whiteDirCount = 0;
					static const int dr[4] = { 0,0,-1,1 };
					static const int dc[4] = { 1,-1,0,0 };

					for (int i = 0; i < 4; i++)
					{
						int xpos = x + dr[i];
						int ypos = y + dc[i];
						if (!isValidPoint({ xpos, ypos })) continue;
						auto it = m_stoneTypeMap.find({ xpos,ypos });
						if (it != m_stoneTypeMap.end() && it->second == White)
							whiteDirCount++;

					}

					if (whiteDirCount >= 2) {
						ok = true;
					}
				}
					break;
				case StoneAbility::jokerExplode:
				{
					if (m_stoneTypeMap.find({ x,y }) != m_stoneTypeMap.end()) return;
					ok = false;
					int BlackCount = 0;
					static const int dr[8] = { 0,0,-1,1,-1,1,1,-1 };
					static const int dc[8] = { 1,-1,0,0,1,1,-1,-1 };

					for (int i = 0; i < 8; i++)
					{
						int xpos = x + dr[i];
						int ypos = y + dc[i];
						if (!isValidPoint({ xpos, ypos })) continue;
						auto it = m_stoneTypeMap.find({ xpos,ypos });
						if (it != m_stoneTypeMap.end() && it->second == Black)
							BlackCount++;

					}

					if (BlackCount >= 5) {
						ok = true;
					}
				}
					break;
 				case StoneAbility::jokerMagnetic:
				{
					if (m_stoneTypeMap.find({ x,y }) != m_stoneTypeMap.end()) return;
					ok = true;
					// 나중에
				}
 					break;
				case StoneAbility::jokerBlackhole:
				{
					if (m_stoneTypeMap.find({ x,y }) != m_stoneTypeMap.end()) return;
					ok = true;
					// 나중에
				}
					break;
				case StoneAbility::jokerShadow:
				{
					if (m_stoneTypeMap.find({ x,y }) != m_stoneTypeMap.end()) return;
					ok = false;

					static const int dr[4] = { -1,-1,1,1 };
					static const int dc[4] = { -1,1,1,-1 };

					for (int i = 0; i < 4; i++)
					{
						int xpos = x + dr[i];
						int ypos = y + dc[i];
						if (!isValidPoint({ xpos, ypos })) continue;
						auto it = m_stoneTypeMap.find({ xpos,ypos });
						if (it != m_stoneTypeMap.end() && it->second == White)
							ok = true;

					}

				}
					break;
				case StoneAbility::jokerWaxseal:
				{
					if (m_stoneTypeMap.find({ x, y }) != m_stoneTypeMap.end())
						break;

					m_stoneTypeMap[{ x, y }] = Black;

					static const int dx[4] = { 1, -1, 0, 0 };
					static const int dy[4] = { 0, 0, 1, -1 };

					std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> seen{};

					for (int dir = 0; dir < 4 && !ok; ++dir)
					{
						int nx = x + dx[dir], ny = y + dy[dir];
						if (!isValidPoint({ nx, ny })) continue;

						auto it = m_stoneTypeMap.find({ nx, ny });
						if (it == m_stoneTypeMap.end() || it->second != White) continue;
						if (seen[nx][ny]) continue;

						std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> visited{};
						std::vector<POINT> group;
						int libs = CountLiberty(nx, ny, group, visited);

						for (const auto& p : group) seen[p.x][p.y] = true;

						if (libs == 0) ok = true;
					}

					m_stoneTypeMap.erase({ x, y });
				}
					break;
				case StoneAbility::jokerOthello:
				{
					if (m_stoneTypeMap.find({ x,y }) != m_stoneTypeMap.end()) return;
					ok = false;
					static const int dr[4] = { 0,0,-1,1 };
					static const int dc[4] = { 1,-1,0,0 };
					for (int dir = 0; dir < 4; ++dir)
					{
						int nx = x + dr[dir];
						int ny = y + dc[dir];

						if (!isValidPoint({ nx, ny })) continue;
						auto it = m_stoneTypeMap.find({ nx, ny });
						if (it == m_stoneTypeMap.end() || it->second != White) continue;

						int cntWhite = 0;
						while (isValidPoint({ nx, ny }))
						{
							it = m_stoneTypeMap.find({ nx, ny });
							if (it == m_stoneTypeMap.end()) break;
							if (it->second != White) break;
							++cntWhite;
							nx += dr[dir];
							ny += dc[dir];
						}
						if (cntWhite >= 3 && isValidPoint({ nx, ny }))
						{
							it = m_stoneTypeMap.find({ nx, ny });
							if (it != m_stoneTypeMap.end() && it->second == Black)
							{
								ok = true;
								break;
							}
						}
					}
				}
					break;
				default:
					ok = true;
					break;
				}
				if (ok) {
					m_hintCells.push_back({ x,y });
					m_mask.insert(Key(x, y));
				}
			}
}


bool BoardManager::IsPlacementAllowed(int gx, int gy) const
{
	return m_mask.count(Key(gx, gy)) > 0;
}

bool BoardManager::IsEmpty(int x, int y)
{
	if (m_board[x][y]) return false;
	return true;
}

bool BoardManager::SelectSacrificeStone(POINT mousePos)
{

	m_selectedPosition = MouseToBoardPosition(mousePos);


	if (!isValidPoint(m_selectedPosition))
	{
		std::cout << "unvaluable position : ("
			<< m_selectedPosition.x << ", "
			<< m_selectedPosition.y << ")\n";
		return false;
	}


	auto& cell = m_board[m_selectedPosition.x][m_selectedPosition.y];
	if (!cell)
	{
		std::cout << "stone null : ("
			<< m_selectedPosition.x << ", "
			<< m_selectedPosition.y << ")\n";
		return false;
	}


	auto itType = m_stoneTypeMap.find(m_selectedPosition);
	if (itType == m_stoneTypeMap.end() || itType->second != StoneType::Black)
		return false;


	if (cell->GetAbility() != StoneAbility::None)
	{
		std::cout << "stone has ability, skip\n";
		return false;
	}


	if (std::find(m_SacrificeGroup.begin(),
		m_SacrificeGroup.end(),
		m_selectedPosition)
		!= m_SacrificeGroup.end())
	{
		return false;
	}


	m_SacrificeGroup.push_back(m_selectedPosition);
	for (auto& item : m_SacrificeGroup)
		std::cout << "(" << item.x << "," << item.y << ") ";
	std::cout << "\n";

	return true;
}


bool BoardManager::checkSacrificeSuccess()
{
	if (m_SacrificeGroup.size() == m_jokerInfoMap[m_stoneAbility].costBlack) 
	{
// 		m_playerInfo.decBlackCount(m_SacrificeGroup.size());
// 		m_playerInfo.incBlackCount(m_jokerInfoMap[m_stoneAbility].returnBlack);
		return true;
	}
	return false;
}


//---------------------------------------------------------------- 조건 충족 모드 함수
bool BoardManager::checkBeforeAbSuccess() 
{
	switch (m_stoneAbility)
	{
	case jokerOmok:
	{
		if (m_useCondGroup.size() != 5)
			return false;

		bool sameRow = true, sameCol = true;
		for (int i = 1; i < 5; ++i) {
			if (m_useCondGroup[i].x != m_useCondGroup[0].x) sameRow = false;
			if (m_useCondGroup[i].y != m_useCondGroup[0].y) sameCol = false;
		}
		if (!sameRow && !sameCol) {
			m_useCondGroup.clear();
			return false;
		}

		int coords[5];
		if (sameRow) {
			for (int i = 0; i < 5; ++i)
				coords[i] = m_useCondGroup[i].y;
		}
		else {
			for (int i = 0; i < 5; ++i)
				coords[i] = m_useCondGroup[i].x;
		}
		std::sort(coords, coords + 5);


		if (coords[4] - coords[0] == 4 &&
			coords[3] - coords[0] == 3 &&
			coords[2] - coords[0] == 2 &&
			coords[1] - coords[0] == 1)
		{
			if (sameRow)                        
			{
				std::sort(m_useCondGroup.begin(), m_useCondGroup.end(),
					[](auto& a, auto& b) { return a.y < b.y; }); 

				for (size_t i = 1; i < 5; ++i)
					if (m_useCondGroup[i].y != m_useCondGroup[i - 1].y + 1) 
						return false;

				m_isVertical = true;
			}
			else  
			{
				std::sort(m_useCondGroup.begin(), m_useCondGroup.end(),
					[](auto& a, auto& b) { return a.x < b.x; });

				for (size_t i = 1; i < 5; ++i)
					if (m_useCondGroup[i].x != m_useCondGroup[i - 1].x + 1) 
						return false;

				m_isVertical = false;       
			}
			return true;
		}
		m_useCondGroup.clear();
		return false;

	}
	case jokerSamok:       
	{
		if (m_useCondGroup.size() != 4)       
			return false;

		bool sameRow = true, sameCol = true;
		for (int i = 1; i < 4; ++i) {     
			if (m_useCondGroup[i].x != m_useCondGroup[0].x) sameRow = false;
			if (m_useCondGroup[i].y != m_useCondGroup[0].y) sameCol = false;
		}
		if (!sameRow && !sameCol) {
			m_useCondGroup.clear();
			return false;
		}

		int coords[4];                           
		if (sameRow) {
			for (int i = 0; i < 4; ++i)
				coords[i] = m_useCondGroup[i].y;
		}
		else {
			for (int i = 0; i < 4; ++i)
				coords[i] = m_useCondGroup[i].x;
		}
		std::sort(coords, coords + 4);


		if (coords[3] - coords[0] == 3 &&
			coords[1] - coords[0] == 1 &&
			coords[2] - coords[0] == 2)
		{
			std::cout << "check success\n";

			RemoveGroup(m_useCondGroup);
			ExitMode();
			return true;
		}
		m_useCondGroup.clear();
		return false;
	}
	case jokerSammok:
	{
		if (m_useCondGroup.size() != 3)
			return false;
		bool sameRow = true, sameCol = true;
		for (int i = 1; i < 3; ++i) {
			if (m_useCondGroup[i].x != m_useCondGroup[0].x) sameRow = false;
			if (m_useCondGroup[i].y != m_useCondGroup[0].y) sameCol = false;
		}
		if (!sameRow && !sameCol) {
			m_useCondGroup.clear();
			return false;
		}
		int coords[3];
		if (sameRow) {
			for (int i = 0; i < 3; ++i) coords[i] = m_useCondGroup[i].y;
		}
		else {
			for (int i = 0; i < 3; ++i) coords[i] = m_useCondGroup[i].x;
		}
		std::sort(coords, coords + 3);
		if (coords[2] - coords[0] == 2 && coords[1] - coords[0] == 1) {
			std::cout << "check success" << std::endl;
			for (const auto& pt : m_useCondGroup) {
				JokerAbilityUse(m_stoneAbility, pt);
			}
			RemoveGroup(m_SacrificeGroup);
			return true;
		}
		m_useCondGroup.clear();
		return false;
	}
	case jokerEvolution:
	{
		if (m_useCondGroup.size() != 1) return false;
		POINT tgt = m_useCondGroup[0];

		StoneAbility next = StoneAbility::None;
		StoneAbility cur = dynamic_pointer_cast<JokerStone>
			(m_board[tgt.x][tgt.y])->GetAbility();

		if (cur == jokerEgg)			next = jokerOstrichEgg;
		else if (cur == jokerFusion)    next = jokerTriunion;
		else if (cur == jokerTriunion)  next = jokerQuadunion;

		if (next != StoneAbility::None && ChangeJokerAbility(tgt, next))
		{
			ExitMode();
			return true;
		}
		m_useCondGroup.clear();
		return false;
	}
	case jokerTeleport: 
	{
		if (m_useCondGroup.size() != 1) return false;
		return true;

	}
	default:
		return true;
	}
	return false;
}

bool BoardManager::SelectUseCond(POINT mousePos)
{
	m_selectedPosition = MouseToBoardPosition(mousePos);

	if (!isValidPoint(m_selectedPosition))
	{
		std::cout << "unvaluable position : (" << m_selectedPosition.x << ", " << m_selectedPosition.y << ")" << std::endl;
		return false;
	} // 보드 밖인지 체크
	if (!m_board[m_selectedPosition.x][m_selectedPosition.y])
	{
		std::cout << "stone null : (" << m_selectedPosition.x << ", " << m_selectedPosition.y << ")" << std::endl;
		return false;
	}
	
	switch (m_stoneAbility)
	{
	case jokerOmok:
	{
		auto itType = m_stoneTypeMap.find(m_selectedPosition);
		if (itType == m_stoneTypeMap.end() || itType->second != StoneType::Black)
			return false;

		auto& cell = m_board[m_selectedPosition.x][m_selectedPosition.y];
		if (cell->GetAbility() != StoneAbility::None )
		{
			std::cout << cell->GetAbility() << "  there is no target \n";
			return false;
		}

		if (std::find(m_useCondGroup.begin(), m_useCondGroup.end(), m_selectedPosition) != m_useCondGroup.end()) return false;
		m_useCondGroup.push_back({ m_selectedPosition.x,m_selectedPosition.y });

		break;
	}
	case jokerSamok:
	case jokerSammok:
	{
		auto& cell = m_board[m_selectedPosition.x][m_selectedPosition.y];

		//auto it = m_stoneTypeMap.find(m_selectedPosition);
		//if (it == m_stoneTypeMap.end() || it->second != Joker) return false; // 조커돌 아니면 패스
		if (cell->GetAbility() == StoneAbility::None)
		{
			std::cout << "stone has ability, skip\n";
			return false;
		}
		if (std::find(m_useCondGroup.begin(), m_useCondGroup.end(), m_selectedPosition) != m_useCondGroup.end()) return false;
		m_useCondGroup.push_back({ m_selectedPosition.x,m_selectedPosition.y });
		break;
	}

	case jokerEvolution: 
	{
		auto& cell = m_board[m_selectedPosition.x][m_selectedPosition.y];
		if (cell->GetAbility() != StoneAbility::jokerFusion &&
			cell->GetAbility() != StoneAbility::jokerTriunion &&
			cell->GetAbility() != StoneAbility::jokerEgg 
			)
		{
			std::cout << cell->GetAbility()<< "  there is no target \n";
			return false;
		}
		if (std::find(m_useCondGroup.begin(), m_useCondGroup.end(), m_selectedPosition) != m_useCondGroup.end()) return false;
		m_useCondGroup.push_back({ m_selectedPosition.x,m_selectedPosition.y });

		break;
	}
	case jokerTeleport:
	{
		auto itType = m_stoneTypeMap.find(m_selectedPosition);
		if (itType == m_stoneTypeMap.end() || itType->second != StoneType::White)
			return false;

		if (std::find(m_useCondGroup.begin(), m_useCondGroup.end(), m_selectedPosition) != m_useCondGroup.end()) return false;
		m_useCondGroup.push_back({ m_selectedPosition.x,m_selectedPosition.y });

		break;
	}

	default:
		return true;
	}
	return false;
}

bool BoardManager::ChangeJokerAbility(POINT pos, StoneAbility newAb)
{
	if (!IsJokerStone(pos)) return false;             

	auto stone = dynamic_pointer_cast<JokerStone>(m_board[pos.x][pos.y]);
	if (!stone) return false;

	stone->UpdateAbility(newAb);                     


	for (auto& p : m_jokerPositions)
		if (p.first == pos) { p.second = newAb; break; }

	return true;
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
