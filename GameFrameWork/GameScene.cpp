#include "cpch.h"
#include "GameScene.h"

#define BOARD_SIZE 15
#define PADDING 107
#define POSX 0
#define POSY 0
#define WIDTH 970
#define HEIGHT 970
#define CELL 54  // 0 기준 54 pxs
#define STONEOFFSET 10

void GameScene::SetUIButton()
{
	m_shopExitButton = std::make_unique<ShopEndButton>(300.0f, -300.0f, 100, 100, "Sample.png");
	m_shopExitButton->SetShowAndActive(false);
	m_buttonList.emplace_back(m_shopExitButton.get());
	m_notUniqueObjectList.emplace_back(m_shopExitButton.get());

	for (int i = 0; i < 3; i++)
	{
		m_shopJokerButtons[i] = make_unique<ShopJokerButton>(-300.0f + (i * 300.0f), 300.0f, 100, 100, "Sample.png");
		m_shopJokerButtons[i]->SetShowAndActive(false);
		m_buttonList.emplace_back(m_shopJokerButtons[i].get());
		m_notUniqueObjectList.emplace_back(m_shopJokerButtons[i].get());
	}
	for (int i = 0; i < 3; i++)
	{
		m_shopJokerButtons[3 + i] = make_unique<ShopJokerButton>(-300.0f + (i * 300.0f), 0.0f, 100, 100, "Sample.png");
		m_shopJokerButtons[3 + i]->SetShowAndActive(false);
		m_buttonList.emplace_back(m_shopJokerButtons[3 + i].get());
		m_notUniqueObjectList.emplace_back(m_shopJokerButtons[3 + i].get());
	}

	unique_ptr<Button> rightUI = std::make_unique<Button>(700.0f, 0.0f, 427, 969, "T_Standard_Right_Base_Glow.png");
	//m_buttonList.emplace_back(rightUI.get());
	m_notUniqueObjectList.emplace_back(rightUI.get());
	m_normalUI.emplace_back(move(rightUI));
	

	unique_ptr<Button> rightUI_Text = std::make_unique<Button>(700.0f, 0.0f, 386, 734, "T_Standard_Right_Slot_Description.png");
	//m_buttonList.emplace_back(rightUI.get());
	m_notUniqueObjectList.emplace_back(rightUI_Text.get());
	m_normalUI.emplace_back(move(rightUI_Text));
	

	//slot간격 170정도
	 // m_buttonList는 기능용
	 // m_UIlist는 그리기용
	 // m_normalUI는 텍스쳐 바꾸기용
	// list로 수정 
	//-----------------------------------------------joker slot------------------------------------
	unique_ptr<Button> jokerSlot1 = std::make_unique<Button>(617.0f, 341.0f, 177, 175, "T_Standard_Right_Slot_Jocker_Glow.png");
	m_buttonList.emplace_back(jokerSlot1.get());
	m_notUniqueObjectList.emplace_back(jokerSlot1.get());
	m_normalUI.emplace_back(move(jokerSlot1));
	

	unique_ptr<Button> jokerSlot2 = std::make_unique<Button>(617.0f, 171.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	m_buttonList.emplace_back(jokerSlot2.get());
	m_notUniqueObjectList.emplace_back(jokerSlot2.get());
	m_normalUI.emplace_back(move(jokerSlot2));
	

	unique_ptr<Button> jokerSlot3 = std::make_unique<Button>(617.0f, 1.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	m_buttonList.emplace_back(jokerSlot3.get());
	m_notUniqueObjectList.emplace_back(jokerSlot3.get());
	m_normalUI.emplace_back(move(jokerSlot3));
	

	unique_ptr<Button> jokerSlot4 = std::make_unique<Button>(617.0f, -172.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	m_buttonList.emplace_back(jokerSlot4.get());
	m_notUniqueObjectList.emplace_back(jokerSlot4.get());
	m_normalUI.emplace_back(move(jokerSlot4));
	

	unique_ptr<Button> jokerSlot5 = std::make_unique<Button>(617.0f, -342.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	m_buttonList.emplace_back(jokerSlot5.get());
	m_notUniqueObjectList.emplace_back(jokerSlot5.get());
	m_normalUI.emplace_back(move(jokerSlot5));
	

	//-----------------------------Item slot---------------------------------------------------
	unique_ptr<Button> itemSlot1 = std::make_unique<Button>(782.5f, 342.0f, 177, 175, "T_Standard_Right_Slot_Item_Glow.png");
	m_buttonList.emplace_back(itemSlot1.get());
	m_notUniqueObjectList.emplace_back(itemSlot1.get());
	m_normalUI.emplace_back(move(itemSlot1));
	

	unique_ptr<Button> itemSlot2 = std::make_unique<Button>(782.5f, 171.0f, 177, 175, "T_Standard_Right_Slot_Item.png");
	m_buttonList.emplace_back(itemSlot2.get());
	m_notUniqueObjectList.emplace_back(itemSlot2.get());
	m_normalUI.emplace_back(move(itemSlot2));
	

	//-----------------------------Passive slot---------------------------------------------------
	unique_ptr<Button> passiveSlot1 = std::make_unique<Button>(782.5f, 1.0f, 177, 175, "T_Standard_Right_Slot_Passive_Glow.png");
	m_buttonList.emplace_back(passiveSlot1.get());
	m_notUniqueObjectList.emplace_back(passiveSlot1.get());
	m_normalUI.emplace_back(move(passiveSlot1));
	

	unique_ptr<Button> passiveSlot2 = std::make_unique<Button>(782.5f, -172.0f, 177, 175, "T_Standard_Right_Slot_Passive.png");
	m_buttonList.emplace_back(passiveSlot2.get());
	m_notUniqueObjectList.emplace_back(passiveSlot2.get());
	m_normalUI.emplace_back(move(passiveSlot2));
	

	unique_ptr<Button> passiveSlot3 = std::make_unique<Button>(782.5f, -342.0f, 177, 175, "T_Standard_Right_Slot_Passive.png");
	m_buttonList.emplace_back(passiveSlot3.get());
	m_notUniqueObjectList.emplace_back(passiveSlot3.get());
	m_normalUI.emplace_back(move(passiveSlot3));
	




	unique_ptr<Button> leftUpUI = std::make_unique<Button>(-720.0f, 388.0f, 427, 199, "T_Standard_Left_Base_Glow.png", 50);
	m_notUniqueObjectList.emplace_back(leftUpUI.get());
	m_normalUI.emplace_back(move(leftUpUI));
	

	unique_ptr<Button> leftUI = std::make_unique<Button>(-720.0f, -100.0f, 427, 769, "T_Standard_Left_Down_Base_Glow.png", 50);
	m_notUniqueObjectList.emplace_back(leftUI.get());
	m_normalUI.emplace_back(move(leftUI));
	




	// ------------------------------------joker button-------------------------------------------
	unique_ptr<JokerButton> jokerButton1 = std::make_unique<JokerButton>(617.0f, 341.0f, 100, 100, "Black.png", 50);
	jokerButton1->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton1.get());
	m_notUniqueObjectList.emplace_back(jokerButton1.get());
	m_jokerButtons.emplace_back(move(jokerButton1));


	unique_ptr<JokerButton> jokerButton2 = std::make_unique<JokerButton>(617.0f, 171.0f, 100, 100, "Black.png", 50);
	jokerButton2->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton2.get());
	m_notUniqueObjectList.emplace_back(jokerButton2.get());
	m_jokerButtons.emplace_back(move(jokerButton2));


	unique_ptr<JokerButton> jokerButton3 = std::make_unique<JokerButton>(617.0f, 1.0f, 100, 100, "Black.png");
	jokerButton3->SetButtonJoker(Joker, None);
	m_buttonList.emplace_back(jokerButton3.get());
	m_notUniqueObjectList.emplace_back(jokerButton3.get());
	m_jokerButtons.emplace_back(move(jokerButton3));


	unique_ptr<JokerButton> jokerButton4 = std::make_unique<JokerButton>(617.0f, -172.0f, 100, 100, "Black.png");
	jokerButton4->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton4.get());
	m_notUniqueObjectList.emplace_back(jokerButton4.get());
	m_jokerButtons.emplace_back(move(jokerButton4));


	unique_ptr<JokerButton> jokerButton5 = std::make_unique<JokerButton>(617.0f, -342.0f, 100, 100, "Black.png");
	jokerButton5->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton5.get());
	m_notUniqueObjectList.emplace_back(jokerButton5.get());
	m_jokerButtons.emplace_back(move(jokerButton5));
}



void GameScene::StartStage()
{
	m_stageNo++;
	m_board.ResetStone();
	int spawn = 3 + (m_stageNo - 1);

	m_board.PlaceRandomStones(spawn);
	m_whiteLeft = m_board.GetStoneTypeAmount(White);
	std::cout << "Stage " << m_stageNo/* << " start, Spawn White Conut : " << spawn << std::endl*/;
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
				for (auto& jokerButton : m_shopJokerButtons)
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
enum asd { test ,test2};

void GameScene::ModeCheck()
{
	m_uiMode = m_board.GetMode();
	//std::cout << m_uiMode << std::endl;

	if (m_uiMode==UIMode::Sacrifice &&m_board.checkSacrificeSuccess())

	{
		m_board.SetMode(UIMode::BeforeUseAbility);
		//m_board.ExitMode();
		std::cout << "Sacrifice clear" << std::endl;
	}
	
	if (m_uiMode == UIMode::BeforeUseAbility&&m_board.checkBeforeAbSuccess())
	{
		if (m_board.GetStoneAbility() == jokerSammok || m_board.GetStoneAbility() == jokerSamok || m_board.GetStoneAbility() == jokerEvolution) { m_board.ExitMode(); return; }
		m_board.SetMode(UIMode::UseAbility);
		std::cout << "BeforeUseAbility clear" << std::endl;
	}


}

void GameScene::InitShop()
{
	m_gameState = GameState::Stage;
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
				bool alreadyExists = false;
				for (auto& jokerButton : m_jokerButtons)
				{
					if (joker.first == jokerButton->GetJokerAbility())
					{
						alreadyExists = true;
						break;
					}
				}
				for (auto& shopJokerButton : m_shopJokerButtons)
				{
					if (joker.first == shopJokerButton->GetJokerAbility())
					{
						alreadyExists = true;
						break;
					}
				}
				if (alreadyExists) continue; // 이미 있는 조커는 제외

				if (joker.second.rarity <= m_stageNo)
				{
					stone = joker.first;
					info = joker.second;
					break;
				}
			}

			if (stone == StoneAbility::None) continue; // 유효한 조커가 없으면 다음으로 넘어감
			m_shopJokerButtons[i]->SetShowAndActive(true);
			m_shopJokerButtons[i]->SetButtonJoker(info, stone);
			m_shopJokerButtons[i]->SetButton(&m_jokerButtons);
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
				bool alreadyExists = false;
				for (auto& jokerButton : m_jokerButtons)
				{
					if (joker.first == jokerButton->GetJokerAbility())
					{
						alreadyExists = true;
						break;
					}
				}
				if (alreadyExists) continue; // 이미 있는 조커는 제외

				if (joker.second.rarity <= m_stageNo)
				{
					stone = joker.first;
					info = joker.second;
					break;
				}
			}

			if (stone == StoneAbility::None) continue; // 유효한 조커가 없으면 다음으로 넘어감
			m_shopJokerButtons[3 + i]->SetShowAndActive(true);
			m_shopJokerButtons[3 + i]->SetButtonJoker(info, stone);
			m_shopJokerButtons[3 + i]->SetButton(&m_jokerButtons);
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
	for (auto& notUniqueObject : m_notUniqueObjectList)
	{
		notUniqueObject->Update(deltaTime);
	}
	m_boardObj->BoardSync();
	//m_board.SyncBlackStoneCount(m_player->GetBlackStone());
	ModeCheck();
	CheckStageClear();
	ChangeThema(m_boardObj->IsBoardChanged());
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

	unique_ptr<BoardObject> boardObj = std::make_unique<BoardObject>(POSX, POSY, WIDTH, HEIGHT, CELL, STONEOFFSET, PADDING);
	m_boardObj = boardObj.get();
	m_objectList.emplace_back(std::move(boardObj));


	//CreateObject::CreateObjectsOutOfScreen(m_objectList, "Leaf6.png", 1920.0f, 1080.0f, 200.0f, 100, 50.0f);

	SetUIButton();
	StartStage();
	InitShop();
}

void GameScene::OnLeave()
{
	std::cout << "Game1 Scene Left" << std::endl;
	Reset();
	m_jokerButtons.clear();
	m_normalUI.clear();
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

			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(0, 1, 0, 0));

		};

	m_commandMap["Back"] = [this]()
		{

			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(0, -1, 0, 0));

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
	if (m_uiMode == UIMode::Normal)
		{
			
			for (auto& button : m_buttonList)
			{
				button->CheckInput(ev);
			}
			if (m_gameState == GameState::Shop) return; // 상점 모드

			if (ev.type == MouseType::LDown)
			{
				if (m_board.GetPlayer().GetBlackCount() <= m_board.GetStoneTypeAmount(Black)) return;
				//std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
				m_board.InputBasedGameLoop(ev.pos);

				std::cout << "Place Black Stone : " << m_board.GetStoneTypeAmount(Black) << " / " << m_board.GetPlayer().GetBlackCount() << std::endl;
			}

			else if (ev.type == MouseType::RDown)
			{
				std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
				m_board.SetStoneType(Joker);
				m_board.SetStoneAbility(jokerTriunion);


				m_board.InputBasedGameLoop(ev.pos);
				std::cout << "Place Black Stone : " << m_board.GetStoneTypeAmount(Black) << " / " << m_board.GetPlayer().GetBlackCount() << std::endl;
				//			std::cout << "Joker Stone Count : " << m_board.GetStoneTypeAmount(Joker) << std::endl;
			}
	}

	else if (m_uiMode == UIMode::Sacrifice)
	{
		if (ev.type == MouseType::LDown)
		{
			m_board.SelectSacrificeStone(ev.pos);
		}
	}

	else if (m_uiMode == UIMode::BeforeUseAbility) //	버튼 종류나 , 방향, 특정 지점을 넘겨줘야 되는 경우 여기 진입
	{
		if (ev.type == MouseType::LDown)
		{
			m_board.SelectUseCond(ev.pos);
		}

	}

	else if (m_uiMode ==UIMode::UseAbility)  //능력 사용 모드
	{
		if (ev.type == MouseType::LDown) {
			m_board.SetStoneType(m_board.GetStoneType());
			m_board.SetStoneAbility(m_board.GetStoneAbility());
			if (m_board.InputBasedGameLoop(ev.pos)) 
			{
				m_board.ExitMode();		// 능력 사용 후 다시 초기화
			}
			std::cout << "Place Black Stone : " << m_board.GetStoneTypeAmount(Black) << " / " << m_board.GetPlayer().GetBlackCount() << std::endl;
		}

	}






	
}

void GameScene::ChangeThema(int thema)
{
	switch (thema)
	{
	case 0: // Forest
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_normalUI)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			rend->SetShaderType("UIColor");
		}
		break;

	case 1: // Space
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_normalUI)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			rend->SetShaderType("UIColor");
		}
		break;

	case 2: // Korea
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_normalUI)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			rend->SetShaderType("UIColor");
		}
		break;

	case 3: // Halloween
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_normalUI)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			rend->SetShaderType("UIColor");
		}
		break;

	case 4: // Cyber
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_normalUI)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			rend->SetShaderType("UIColor");
		}
		break;

	default: // 기본 테마
		break;
	}
}

