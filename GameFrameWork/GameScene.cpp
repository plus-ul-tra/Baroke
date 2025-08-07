#include "cpch.h"
#include "GameScene.h"

#define BOARD_SIZE 15
#define PADDING 72
#define POSX 0
#define POSY 0
#define WIDTH 900
#define HEIGHT 900
#define CELL 54  // 0 기준 54 pxs
#define STONEOFFSET 10

void GameScene::SetUIJokerButton()
{
	m_shopExitButton = std::make_unique<ShopEndButton>(300.0f, -300.0f, 100, 100, "Sample.png");
	m_shopExitButton->SetShowAndActive(false);
	m_buttonList.emplace_back(m_shopExitButton.get());
	m_UIList.emplace_back(m_shopExitButton.get());

	for (int i = 0; i < 3; i++)
	{
		jokerButtons[i] = make_unique<ShopJokerButton>(-300.0f + (i * 300.0f), 300.0f, 100, 100, "Sample.png");
		jokerButtons[i]->SetShowAndActive(false);
		m_buttonList.emplace_back(jokerButtons[i].get());
		m_UIList.emplace_back(jokerButtons[i].get());
	}
	for (int i = 0; i < 3; i++)
	{
		jokerButtons[3 + i] = make_unique<ShopJokerButton>(-300.0f + (i * 300.0f), 0.0f, 100, 100, "Sample.png");
		jokerButtons[3 + i]->SetShowAndActive(false);
		m_buttonList.emplace_back(jokerButtons[3 + i].get());
		m_UIList.emplace_back(jokerButtons[3 + i].get());
	}

	unique_ptr<Button> rightUI = std::make_unique<Button>(700.0f, 0.0f, 418, 973.0f, "T_Right_UI.png");
	m_buttonList.emplace_back(rightUI.get());
	m_objectList.emplace_back(std::move(rightUI));


	unique_ptr<JokerButton> jokerButton1 = std::make_unique<JokerButton>(600.0f, 300.0f, 100, 100, "jokerOmok.png", 50);
	jokerButton1->SetButtonJoker(Joker, jokerOmok);
	m_buttonList.emplace_back(jokerButton1.get());
	m_UIList.emplace_back(std::move(jokerButton1));

	unique_ptr<JokerButton> jokerButton2 = std::make_unique<JokerButton>(600.0f, 150.0f, 100, 100, "jokerFusion.png", 50);
	jokerButton2->SetButtonJoker(Joker, jokerFusion);
	m_buttonList.emplace_back(jokerButton2.get());
	m_UIList.emplace_back(std::move(jokerButton2));

	unique_ptr<JokerButton> jokerButton3 = std::make_unique<JokerButton>(600.0f, 0.0f, 100, 100, "jokerTriunion.png");
	jokerButton3->SetButtonJoker(Joker, jokerTriunion);
	m_jokerButtons[2] = jokerButton3.get();
	m_buttonList.emplace_back(jokerButton3.get());
	m_UIList.emplace_back(std::move(jokerButton3));

	unique_ptr<JokerButton> jokerButton4 = std::make_unique<JokerButton>(600.0f, -150.0f, 100, 100, "jokerEvolution.png");
	jokerButton4->SetButtonJoker(Joker, jokerEvolution);
	m_jokerButtons[3] = jokerButton4.get();
	m_buttonList.emplace_back(jokerButton4.get());
	m_UIList.emplace_back(std::move(jokerButton4));

	unique_ptr<JokerButton> jokerButton5 = std::make_unique<JokerButton>(600.0f, -300.0f, 100, 100, "jokerTeleport.png");
	jokerButton5->SetButtonJoker(Joker, jokerTeleport);
	m_jokerButtons[4] = jokerButton5.get();
	m_buttonList.emplace_back(jokerButton5.get());
	m_UIList.emplace_back(std::move(jokerButton5));
}



void GameScene::StartStage()
{
	m_stageNo++;
	m_board.ResetStone();
	int spawn = 3 + (m_stageNo - 1);

	m_board.PlaceRandomStones(spawn);
	m_whiteLeft = m_board.GetStoneTypeAmount(White);
	std::cout << "Stage " << m_stageNo << " start, Spawn White Conut : " << spawn << std::endl;
}

void GameScene::CheckStageClear()
{
	if (m_board.GetStoneTypeAmount(White) <= 0)  // 스테이지 클리어
	{
		SceneManager::GetInstance().ChangePostProcessing("CRTFilter");

		if (m_gameState == GameState::Stage)
		{
			m_gameState = GameState::ShopEnter;
		}
		if (m_gameState == GameState::ShopEnter)
		{
			m_gameState = GameState::Shop;
			ShopStage();
			m_shopExitButton->SetShowAndActive(true);
		}

		if (m_gameState == GameState::Shop && m_shopExitButton)
		{
			if (m_shopExitButton->IsEndButtonPressed())
			{
				m_gameState = GameState::Stage;

				m_shopExitButton->SetShowAndActive(false);
				for (auto& jokerButton : jokerButtons)
				{
					jokerButton->SetShowAndActive(false);
				}
			}
		}

		if (m_gameState == GameState::Stage)
		{
			StartStage();
		}
	}
}



void GameScene::ModeCheck()
{
	m_uiMode = m_board.GetMode();


	if (m_uiMode==UIMode::Sacrifice &&m_board.checkSacrificeSuccess())

	{
		m_board.SetMode(UIMode::BeforeUseAbility);
		//m_board.ExitMode();
		std::cout << "Sacrifice clear" << std::endl;
	}
	
	if (m_uiMode == UIMode::BeforeUseAbility&&m_board.checkBeforeAbSuccess())
	{
		m_board.SetMode(UIMode::UseAbility);
	}
}

void GameScene::InitShop()
{
	for (auto& jokers : m_jokerInfoMap)
	{
		if (jokers.second.isStone) m_shopStones.push_back({ jokers.first, jokers.second });
		else m_shopItems.push_back({ jokers.first, jokers.second });
	}
}

void GameScene::ShopStage()
{
	m_board.ResetStone();
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> dist(0, 100);

	for (int i = 0; i < 3; i++)
	{

		if (dist(rng) < m_shopRng[i])
		{
			StoneAbility stone = StoneAbility::None;
			JokerStoneInfo info = JokerStoneInfo();
			shuffle(m_shopStones.begin(), m_shopStones.end(), rng);
			for (auto& joker : m_shopStones)
			{
				if (joker.second.rarity <= m_stageNo)
				{
					stone = joker.first;
					info = joker.second;
					break;
				}
			}

			jokerButtons[i]->SetShowAndActive(true);
			jokerButtons[i]->SetButtonJoker(info, stone);
		}
	}
	for (int i = 0; i < 3; i++)
	{

		if (dist(rng) < m_shopRng[i])
		{
			StoneAbility stone = StoneAbility::None;
			JokerStoneInfo info = JokerStoneInfo();
			shuffle(m_shopItems.begin(), m_shopItems.end(), rng);
			for (auto& joker : m_shopItems)
			{
				if (joker.second.rarity <= m_stageNo)
				{
					stone = joker.first;
					info = joker.second;
					break;
				}
			}

			jokerButtons[3 + i]->SetShowAndActive(true);
			jokerButtons[3 + i]->SetButtonJoker(info, stone);
		}
	}
}

void GameScene::Initialize()
{
	std::cout << "Game Scene Init" << std::endl;
	KeyCommandMapping();

}

void GameScene::FixedUpdate(double fixedDeltaTime)
{
	/*for (auto& a : m_objectList) {
		a->FixedUpdate(deltaTime);
	}*/
}

void GameScene::Update(double deltaTime)
{
	for (auto& object : m_objectList)
	{
		object->Update(deltaTime);
	}
	for (auto& UI : m_UIList)
	{
		UI->Update(deltaTime);
	}
	m_boardObj->BoardSync();
	m_board.SyncBlackStoneCount(m_player->GetBlackStone());
	ModeCheck();
	CheckStageClear();
}

void GameScene::LateUpdate(double deltaTime)
{
	/*for (auto& a : m_objectList) {
		a->LateUpdate(deltaTime);
	}*/
	if (!DirectX::XMVector3Equal(m_moveDir, DirectX::XMVectorZero()) && m_player)
	{
		DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(m_moveDir);
		m_player->Move(dir, deltaTime);
	}

	m_moveDir = DirectX::XMVectorZero();
}

void GameScene::OnEnter()
{

	std::cout << "Game1 Scene OnEnter" << std::endl;

	unique_ptr<BackGround> backGround = std::make_unique<BackGround>(0.0f, 0.0f, 1920.0f, 1080.0f);
	m_objectList.emplace_back(std::move(backGround));


	auto playerObject = std::make_unique<Player>(
		0.0f, 0.0f, 50.0f, 50.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) // 녹색
	);
	m_player = playerObject.get();
	m_objectList.push_back(std::move(playerObject));

	unique_ptr<Button> leftUI = std::make_unique<Button>(-720.0f, 100.0f, 400.0f, 700.0f, "T_Left_Down.png", 50);
	m_buttonList.emplace_back(leftUI.get());
	m_objectList.emplace_back(std::move(leftUI));

	unique_ptr<BoardObject> boardObj = std::make_unique<BoardObject>(POSX, POSY, WIDTH, HEIGHT, CELL, STONEOFFSET, PADDING);
	m_boardObj = boardObj.get();
	m_objectList.emplace_back(std::move(boardObj));

// 	unique_ptr<Button> xx = std::make_unique<Button>(0.0f, 200.0f, 461.0f, 141.0f, "tool1.png", 50);
// 	m_buttonList.emplace_back(xx.get());
// 	m_objectList.emplace_back(std::move(xx));
// 
// 	unique_ptr<Button> yy = std::make_unique<Button>(0.0f, -200.0f, 461.0f, 141.0f, "tool2.png", 50);
// 	m_buttonList.emplace_back(yy.get());
// 	m_objectList.emplace_back(std::move(yy));
	SetUIJokerButton();

	StartStage();
	InitShop();
}

void GameScene::OnLeave()
{
	std::cout << "Game1 Scene Left" << std::endl;
	Reset();
}

void GameScene::OnCommand(std::string& cmd)
{
	auto it = m_commandMap.find(cmd);
	if (it != m_commandMap.end())
	{
		it->second(); // 함수 실행
	}
	else
	{
		std::cout << "Unknown Command: " << cmd << std::endl;
	}
}

void GameScene::KeyCommandMapping()
{
	m_commandMap["Escape"] = [this]()
		{
			m_board.ExitMode();
			// 추후 일시정지/재개 로직 여기에
		};

	m_commandMap["F1"] = [this]()
		{
			std::cout << "F1 Command Received: Changing to Title Scene" << std::endl;
			SceneManager::GetInstance().ChangeScene(std::string("Title"));
		};

	m_commandMap["F2"] = [this]()
		{
			SceneManager::GetInstance().ChangePostProcessing("PassThrough");
		};

	m_commandMap["F3"] = [this]()
		{
			//m_board.PlaceRandomStones(3);
		};

	m_commandMap["F4"] = [this]()
		{
			m_board.ResetStone();
		};

	m_commandMap["F5"] = [this]()
		{
			SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
		};

	m_commandMap["Go"] = [this]()
		{

			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(0, -1, 0, 0));

		};

	m_commandMap["Back"] = [this]()
		{

			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(0, 1, 0, 0));

		};

	m_commandMap["Left"] = [this]()
		{

			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(-1, 0, 0, 0));

		};

	m_commandMap["Right"] = [this]()
		{

			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(1, 0, 0, 0));

		};
}

void GameScene::OnInput(const MouseEvent& ev)
{
	if (m_uiMode ==UIMode::UseAbility)  //능력 사용 모드
	{
		if (ev.type == MouseType::LDown) {
			m_board.SetStoneType(m_board.GetStoneType());
			m_board.SetStoneAbility(m_board.GetStoneAbility());
			if (m_board.InputBasedGameLoop(ev.pos)) 
			{
				m_board.ExitMode();		// 능력 사용 후 다시 초기화
			}
		}
	}

	else if (m_uiMode == UIMode::BeforeUseAbility) //	버튼 종류나 , 방향, 특정 지점을 넘겨줘야 되는 경우 여기 진입
	{
		if (ev.type == MouseType::LDown)
		{
			m_board.SelectUseCond(ev.pos);
		}

	}

	else if (m_uiMode == UIMode::Sacrifice)
	{
		if (ev.type == MouseType::LDown)
		{
			m_board.SelectSacrificeStone(ev.pos);
		}
	}

	else if(m_uiMode == UIMode::Normal)
	{

		for (auto& button : m_buttonList)
		{
			button->CheckInput(ev);
		}

		if (ev.type == MouseType::LDown)
		{
			if (m_player->GetBlackStone() <= m_board.GetStoneTypeAmount(Black)) return;
			std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
			m_board.InputBasedGameLoop(ev.pos);
//			std::cout << "Black Stone Count : " << m_board.GetStoneTypeAmount(Black) << std::endl;
		}


		else if (ev.type == MouseType::RDown)
		{
			std::cout << ev.pos.x << " " << ev.pos.y << std::endl;

			m_board.SetStoneType(Joker);
			m_board.SetStoneAbility(jokerMrchan);


			m_board.InputBasedGameLoop(ev.pos);
//			std::cout << "Joker Stone Count : " << m_board.GetStoneTypeAmount(Joker) << std::endl;
		}


	}





}

