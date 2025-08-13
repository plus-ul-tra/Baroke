#include "cpch.h"
#include "TutoScene.h"


#define BOARD_SIZE 15
#define PADDING 106
#define POSX 0
#define POSY 0
#define WIDTH 970
#define HEIGHT 970
#define CELL 54  // 0 기준 54 pxs
#define STONEOFFSET 10
using namespace std;

void TutoScene::initGame()
{
	SetUIButton();						// 버튼들 초기화
	SetHintpool();						// 힌트 풀 초기화
	m_board.GetPlayer().TutoinitGame();	// 플레이어 초기화
	ShopStage();
}

void TutoScene::SetUIButton()
{




	//--------------------------UI일반----------------------------------
	unique_ptr<Button> rightUI = std::make_unique<Button>(700.0f, 0.0f, 427, 969, "T_Standard_Right_Base_Glow.png");
	m_notUniqueObjectList.emplace_back(rightUI.get());
	//m_normalUI.emplace_back(move(rightUI));
	rightUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_rightUI = move(rightUI);

	unique_ptr<Button> desc = std::make_unique<Button>(700.0f, 0.0f, 386, 734, "T_Standard_Right_Slot_Description.png");
	m_notUniqueObjectList.emplace_back(desc.get());
	//m_normalUI.emplace_back(move(desc));
	desc->GetComponent<BitmapRender3D>()->SetShaderType("Holo");
	m_desc = move(desc);

	unique_ptr<Button> leftUpUI = std::make_unique<Button>(-720.0f, 388.0f, 427, 199, "T_Standard_Left_Base_Glow.png");
	m_notUniqueObjectList.emplace_back(leftUpUI.get());
	//leftUpUI->AddComponent<UIText>(-680.0f, 440.0f, 100.0f, 100.0f, 2);
	leftUpUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_leftUpUI = move(leftUpUI);


	unique_ptr<Button> leftUI = std::make_unique<Button>(-720.0f, -100.0f, 427, 769, "T_Standard_Left_Down_Base_Glow.png");
	m_notUniqueObjectList.emplace_back(leftUI.get());
	//m_normalUI.emplace_back(move(leftUI));
	leftUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_leftUI = move(leftUI);


	//--------------------------좌측 조커 선택칸----------------------------------

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int slot = i * 4 + j;
			m_shopJokerButtons[slot] = make_unique<ShopJokerButton>(-850 + (j * 85.f), 180.0f - (100.f * i), 70, 70, "Sample.png", true);
			m_shopJokerButtons[slot]->SetShowAndActive(true);
			m_buttonList.emplace_back(m_shopJokerButtons[slot].get());
			m_notUniqueObjectList.emplace_back(m_shopJokerButtons[slot].get());
		}
	}


	//-- unchanged----------------------
	// 장식품
	unique_ptr<Button> stageText = std::make_unique<Button>(-720.0f, 388.0f, 317, 58, "TUTORIALTITLE.png");
	m_notUniqueObjectList.emplace_back(stageText.get());
	stageText->GetComponent<BitmapRender3D>()->SetShaderType("Holo");
	m_useless.emplace_back(move(stageText));

	unique_ptr<SceneChangeButton> exitText = std::make_unique<SceneChangeButton>(-720.0f, -390.0f, 167, 33, "TUTORIALEXIT.png", "Title", buttonType::TutorialToTitle);
	m_buttonList.emplace_back(exitText.get());
	m_notUniqueObjectList.emplace_back(exitText.get());
	//exitText->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_useless.emplace_back(move(exitText));

	unique_ptr<Button> cyberAlpha = std::make_unique<Button>(0.0f, 0.0f, 900, 900, "CyberAlpha.png");
	m_notUniqueObjectList.emplace_back(cyberAlpha.get());
	cyberAlpha->GetComponent<BitmapRender3D>()->SetShaderType("Holo");
	cyberAlpha->GetComponent<BitmapRender3D>()->SetActive(false);
	m_cyber = move(cyberAlpha);

	//slot간격 170정도
	 // m_buttonList는 기능용
	 // m_UIlist는 그리기용
	 // m_normalUI는 텍스쳐 바꾸기용
	// list로 수정 
	//-----------------------------------------------joker slot------------------------------------
	unique_ptr<Button> jokerSlot1 = std::make_unique<Button>(617.0f, 341.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot1->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png", 0);
	m_buttonList.emplace_back(jokerSlot1.get());
	m_notUniqueObjectList.emplace_back(jokerSlot1.get());
	m_jokerSlot.emplace_back(move(jokerSlot1));


	unique_ptr<Button> jokerSlot2 = std::make_unique<Button>(617.0f, 171.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot2->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png", 1);
	m_buttonList.emplace_back(jokerSlot2.get());
	m_notUniqueObjectList.emplace_back(jokerSlot2.get());
	m_jokerSlot.emplace_back(move(jokerSlot2));


	unique_ptr<Button> jokerSlot3 = std::make_unique<Button>(617.0f, 1.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot3->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png", 2);
	m_buttonList.emplace_back(jokerSlot3.get());
	m_notUniqueObjectList.emplace_back(jokerSlot3.get());
	m_jokerSlot.emplace_back(move(jokerSlot3));


	unique_ptr<Button> jokerSlot4 = std::make_unique<Button>(617.0f, -172.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot4->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png", 3);
	m_buttonList.emplace_back(jokerSlot4.get());
	m_notUniqueObjectList.emplace_back(jokerSlot4.get());
	m_jokerSlot.emplace_back(move(jokerSlot4));


	unique_ptr<Button> jokerSlot5 = std::make_unique<Button>(617.0f, -342.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot5->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png", 4);
	m_buttonList.emplace_back(jokerSlot5.get());
	m_notUniqueObjectList.emplace_back(jokerSlot5.get());
	m_jokerSlot.emplace_back(move(jokerSlot5));


	//-----------------------------Item slot---------------------------------------------------
	unique_ptr<Button> itemSlot1 = std::make_unique<Button>(782.5f, 342.0f, 177, 175, "T_Standard_Right_Slot_Item_Glow.png");
	m_buttonList.emplace_back(itemSlot1.get());
	m_notUniqueObjectList.emplace_back(itemSlot1.get());
	m_itemSlot.emplace_back(move(itemSlot1));


	unique_ptr<Button> itemSlot2 = std::make_unique<Button>(782.5f, 171.0f, 177, 175, "T_Standard_Right_Slot_Item.png");
	m_buttonList.emplace_back(itemSlot2.get());
	m_notUniqueObjectList.emplace_back(itemSlot2.get());
	m_itemSlot.emplace_back(move(itemSlot2));


	//-----------------------------Passive slot---------------------------------------------------
	unique_ptr<Button> passiveSlot1 = std::make_unique<Button>(782.5f, 1.0f, 177, 175, "T_Standard_Right_Slot_Passive_Glow.png");
	m_buttonList.emplace_back(passiveSlot1.get());
	m_notUniqueObjectList.emplace_back(passiveSlot1.get());
	m_passiveSlot.emplace_back(move(passiveSlot1));


	unique_ptr<Button> passiveSlot2 = std::make_unique<Button>(782.5f, -172.0f, 177, 175, "T_Standard_Right_Slot_Passive.png");
	m_buttonList.emplace_back(passiveSlot2.get());
	m_notUniqueObjectList.emplace_back(passiveSlot2.get());
	m_passiveSlot.emplace_back(move(passiveSlot2));


	unique_ptr<Button> passiveSlot3 = std::make_unique<Button>(782.5f, -342.0f, 177, 175, "T_Standard_Right_Slot_Passive.png");
	m_buttonList.emplace_back(passiveSlot3.get());
	m_notUniqueObjectList.emplace_back(passiveSlot3.get());
	m_passiveSlot.emplace_back(move(passiveSlot3));



	// ------------------------------------joker button-------------------------------------------
	unique_ptr<JokerButton> jokerButton1 = std::make_unique<JokerButton>(617.0f, 341.0f, 100, 100, "T_Blackstone.png", 50);
	jokerButton1->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton1.get());
	m_notUniqueObjectList.emplace_back(jokerButton1.get());
	m_jokerButtons.emplace_back(move(jokerButton1));


	unique_ptr<JokerButton> jokerButton2 = std::make_unique<JokerButton>(617.0f, 171.0f, 100, 100, "T_Blackstone.png", 50);
	jokerButton2->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton2.get());
	m_notUniqueObjectList.emplace_back(jokerButton2.get());
	m_jokerButtons.emplace_back(move(jokerButton2));


	unique_ptr<JokerButton> jokerButton3 = std::make_unique<JokerButton>(617.0f, 1.0f, 100, 100, "T_Blackstone.png");
	jokerButton3->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton3.get());
	m_notUniqueObjectList.emplace_back(jokerButton3.get());
	m_jokerButtons.emplace_back(move(jokerButton3));


	unique_ptr<JokerButton> jokerButton4 = std::make_unique<JokerButton>(617.0f, -172.0f, 100, 100, "T_Blackstone.png");
	jokerButton4->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton4.get());
	m_notUniqueObjectList.emplace_back(jokerButton4.get());
	m_jokerButtons.emplace_back(move(jokerButton4));


	unique_ptr<JokerButton> jokerButton5 = std::make_unique<JokerButton>(617.0f, -342.0f, 100, 100, "T_Blackstone.png");
	jokerButton5->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton5.get());
	m_notUniqueObjectList.emplace_back(jokerButton5.get());
	m_jokerButtons.emplace_back(move(jokerButton5));

	// ------------------------------------joker Active button-------------------------------------------
	unique_ptr<JokerButton> jokerButton6 = std::make_unique<JokerButton>(781.0f, 1.0f, 100, 100, "T_Blackstone.png");
	jokerButton6->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton6.get());
	m_notUniqueObjectList.emplace_back(jokerButton6.get());
	m_jokerButtons.emplace_back(move(jokerButton6));


	unique_ptr<JokerButton> jokerButton7 = std::make_unique<JokerButton>(781.0f, -172.0f, 100, 100, "T_Blackstone.png");
	jokerButton7->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton7.get());
	m_notUniqueObjectList.emplace_back(jokerButton7.get());
	m_jokerButtons.emplace_back(move(jokerButton7));


	unique_ptr<JokerButton> jokerButton8 = std::make_unique<JokerButton>(781.0f, -342.0f, 100, 100, "T_Blackstone.png");
	jokerButton8->SetButtonJoker(Black, None);
	m_buttonList.emplace_back(jokerButton8.get());
	m_notUniqueObjectList.emplace_back(jokerButton8.get());
	m_jokerButtons.emplace_back(move(jokerButton8));


}


void TutoScene::CheckSlot()
{
	int index = Mediator::GetInstance().GetSlotIndex();
	if (index != -1 && m_lastIndex != index) {
		for (int i = 0; i < 5; i++) {
			if (i == index) {
				m_jokerSlot[i]->Selected();
				m_lastIndex = index;
			}
			else {
				m_jokerSlot[i]->UnSelected();
			}
		}
	}
}

void TutoScene::ModeCheck()
{
	m_uiMode = m_board.GetMode();
	//std::cout << m_uiMode << std::endl;

	if (m_uiMode == UIMode::Sacrifice && m_board.checkSacrificeSuccess())
	{
		m_board.SetMode(UIMode::BeforeUseAbility);
		std::cout << "Sacrifice clear" << std::endl;
	}

	if (m_uiMode == UIMode::BeforeUseAbility && m_board.checkBeforeAbSuccess())
	{
		SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
		if (m_board.GetStoneAbility() == jokerSammok ||
			m_board.GetStoneAbility() == jokerSamok ||
			m_board.GetStoneAbility() == jokerEvolution)
		{
			std::cout << "jokerevol" << std::endl;
			m_board.ExitMode();

			return;
		}

		m_board.SetMode(UIMode::UseAbility);
		m_board.ComputePlacementHints(m_board.GetStoneAbility());
		std::cout << "BeforeUseAbility clear" << std::endl;
	}

	if (m_uiMode == UIMode::UseAbility)
	{
		//std::cout << "UseAbility" << std::endl;

		SyncPlacementHintsToPool();
	}


}

void TutoScene::CRTAccess()
{
	if (m_uiMode == UIMode::Normal) { SceneManager::GetInstance().ChangePostProcessing("CRTFilter"); }
	else if (m_uiMode == UIMode::Sacrifice) { SceneManager::GetInstance().ChangePostProcessing("CRTRed"); }
	else if (m_uiMode == UIMode::BeforeUseAbility) { SceneManager::GetInstance().ChangePostProcessing("CRTGreen"); }
	else if (m_uiMode == UIMode::UseAbility) { SceneManager::GetInstance().ChangePostProcessing("CRTFilter"); }

}


void TutoScene::ShopStage()
{
	StoneAbility stone = StoneAbility::None;
	JokerStoneInfo info = JokerStoneInfo();
	int i = 0;
	for (auto& jokers : m_jokerInfoMap)
	{
		m_shopStones.emplace_back(jokers.first, jokers.second);
	}

	for (auto& joker : m_shopStones) 
	{

		if(joker.first == None) continue;
		stone = joker.first;
		info = joker.second;
		m_shopJokerButtons[i]->SetShowAndActive(true);
		m_shopJokerButtons[i]->SetButtonJoker(info, stone);
		m_shopJokerButtons[i]->SetButton(&m_jokerButtons);
		i++;
	
	}
}

void TutoScene::SyncPlacementHintsToPool()
{
	const auto& hints = m_board.GetHints();

	for (auto& h : m_hintPool) h->Show(false);

	const size_t n = std::min(hints.size(), m_hintPool.size());
	for (size_t i = 0; i < n; ++i)
	{
		POINT screen = m_board.BoardToScreenPosition(hints[i]);

		float sx = static_cast<float>(screen.x);
		float sy = static_cast<float>(screen.y);


		m_hintPool[i]->SetScreenPos(sx + CELL / 2 - 1, sy + CELL / 2);
		m_hintPool[i]->Show(true);
	}
}


void TutoScene::SetHintpool()
{

	const int HINT_POOL_SIZE = 225;
	const float hintW = (float)CELL * 0.3f;
	const float hintH = (float)CELL * 0.3f;
	const std::string hintTex = "T_White_6x6.png";
	std::cout << "HighTest_1 Create Success" << std::endl;

	m_hintPool.reserve(HINT_POOL_SIZE);
	for (int i = 0; i < HINT_POOL_SIZE; ++i) {
		auto up = std::make_unique<HintMark>(hintW, hintH, hintTex, 100);
		up->Show(false);
		m_notUniqueObjectList.emplace_back(up.get());
		m_hintPool.emplace_back(std::move(up));
	}

}

void TutoScene::Initialize()
{
	std::cout << "Tutorial Scene Initialize" << std::endl;
	KeyCommandMapping();
}

void TutoScene::FixedUpdate(double fixedDeltaTime)
{
	/*for (auto& a : m_objectList) {
		a->FixedUpdate(deltaTime);
	}*/
}

void TutoScene::Update(double deltaTime)
{
	m_filterElsapsedTime += deltaTime;

	m_boardObj->BoardSync();
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
		if (SceneManager::GetInstance().IsExit()) break;
	}

	for (auto& button : m_shopJokerButtons) 
	{
		button->SetShowAndActive(true);
	}

	if (!DirectX::XMVector3Equal(m_moveDir, DirectX::XMVectorZero()) && m_player)
	{
		DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(m_moveDir);
		m_player->Move(dir, deltaTime);
	}

	m_moveDir = DirectX::XMVectorZero();


	m_gameStateDelayElapsed += deltaTime;

	CheckSlot();
	ModeCheck();


	if (!m_isExitrQueue && m_filterElsapsedTime > 0.0f)
	{
		if (m_isFilterQueue) m_isFilterQueue = false;
		CRTAccess();
	}

	if (SceneManager::GetInstance().IsExit())
	{
		if (!m_isExitrQueue)
		{
			m_isExitrQueue = true;
			m_filterElsapsedTime = -1.25f;
			SceneManager::GetInstance().ChangePostProcessing("CRTOff");
		}
		if (m_isExitrQueue && m_filterElsapsedTime > 0.0f)
		{
			m_isExitrQueue = false;
			SceneManager::GetInstance().ChangeSceneToNext();
		}
	}

	//if (m_isFilterQueue && m_filterElsapsedTime > 0.8f)
	//{
	//	m_isFilterQueue = false;
	//	CRTAccess();
	//}

	//if (SceneManager::GetInstance().IsExit())
	//{
	//	if (!m_isExitrQueue)
	//	{
	//		m_isExitrQueue = true;
	//		m_filterElsapsedTime = 0.0f;
	//		SceneManager::GetInstance().ChangePostProcessing("CRTOff");
	//	}
	//	if (m_isExitrQueue && m_filterElsapsedTime > 1.3f)
	//	{
	//		m_isExitrQueue = false;
	//		SceneManager::GetInstance().ChangeSceneToNext();
	//	}
	//}
}

void TutoScene::LateUpdate(double deltaTime)
{
}

void TutoScene::OnEnter()
{
	std::cout << "Tuto Scene OnEnter" << std::endl;

	unique_ptr<BackGround> backGround = std::make_unique<BackGround>(0.0f, 0.0f, 1920.0f, 1080.0f);
	m_objectList.emplace_back(std::move(backGround));

	XMFLOAT4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
	Mediator::GetInstance().SetBackGroundColor(color, color);
	SceneManager::GetInstance().ChangePostProcessing("CRTOn");
	m_filterElsapsedTime = -0.75f;
	m_isFilterQueue = true;
	m_isExitrQueue = false;

	auto playerObject = std::make_unique<Player>(
		0.0f, 0.0f, 100.0f, 100.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) // 녹색
	);
	m_player = playerObject.get();
	m_objectList.push_back(std::move(playerObject));

	unique_ptr<BoardObject> boardObj = std::make_unique<BoardObject>(POSX, POSY, WIDTH, HEIGHT, CELL, STONEOFFSET, PADDING);
	m_boardObj = boardObj.get();
	m_objectList.emplace_back(std::move(boardObj));


	m_lastIndex = -1;

	// 사운드
	m_channel->stop();
	m_bgm = nullptr;
	m_channel = nullptr;
	m_bgm = SoundManager::GetInstance().GetSound("MainBGM.mp3");
	m_bgm->setMode(FMOD_LOOP_NORMAL);
	m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);

	initGame();
}

void TutoScene::OnLeave()
{
	std::cout << "Tutorial Scene Left" << std::endl;
	Reset();
	m_useless.clear();
	m_jokerButtons.clear();
	m_jokerSlot.clear();
	m_itemSlot.clear();
	m_passiveSlot.clear();
	m_hintPool.clear();
	m_shopStones.clear();
	m_lastIndex = -1;


	// 사운드 초기화
	m_channel->stop();
	m_bgm = nullptr;
	m_channel = nullptr;
}

void TutoScene::OnCommand(std::string& cmd)
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

void TutoScene::KeyCommandMapping()
{
	m_commandMap["Escape"] = [this]()
		{
			m_board.ClearHints();
			m_board.ExitMode();
			SyncPlacementHintsToPool();
			SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
			// 추후 일시정지/재개 로직 여기에
		};

	m_commandMap["F1"] = [this]()
		{
			SceneManager::GetInstance().ChangeScene(std::string("Title"));
		};

	m_commandMap["F2"] = [this]()
		{
			m_board.ResetStone();
		};

	m_commandMap["F3"] = [this]()
		{
			m_board.PlaceRandomStones(5);
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

void TutoScene::OnInput(const MouseEvent& ev)
{
	for (auto& button : m_buttonList)
	{
		button->CheckInput(ev);
	}

	if (m_uiMode == UIMode::Normal)
	{
		if (ev.type == MouseType::LDown)
		{
			if (m_board.GetPlayer().GetBlackCount() <= m_board.GetStoneTypeAmount(Black)) return;
			m_board.InputBasedGameLoop(ev.pos);
			std::cout << "Place Black Stone : " << m_board.GetStoneTypeAmount(Black) << " / " << m_board.GetPlayer().GetBlackCount() << std::endl;
		}


		else if (ev.type == MouseType::RDown)
		{
			std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
			m_board.SetStoneType(White);
			m_board.SetStoneAbility(None);
			if (!m_board.InputBasedGameLoop(ev.pos)) 
			{
				m_board.SetStoneType(Black);
				m_board.SetStoneAbility(None);
			}
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
		SceneManager::GetInstance().ChangePostProcessing("CRTGreen");
		if (ev.type == MouseType::LDown)
		{

			m_board.SelectUseCond(ev.pos);
		}

	}
	else if (m_uiMode == UIMode::UseAbility)
	{
		if (ev.type == MouseType::LDown) {
			POINT grid = m_board.MouseToBoardPosition(ev.pos);

			if (!m_board.IsPlacementAllowed(grid.x, grid.y))
				return;

			bool isBlackhole = (m_board.GetStoneAbility() == jokerBlackhole);

			if (m_board.InputBasedGameLoop(ev.pos))
			{
				if (isBlackhole)
				{
					m_filterElsapsedTime = -3.0f;
				}
				m_board.ClearHints();
				m_board.ExitMode();
				//SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
				SyncPlacementHintsToPool();
			}
			std::cout << "Place Black Stone : " << m_board.GetStoneTypeAmount(Black)
				<< " / " << m_board.GetPlayer().GetBlackCount() << std::endl;
		}
	}
}

// void TutoScene::ChangeThema(int thema)
// {
// 	switch (thema)
// 	{
// 	case 1: // Forest
// 		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
// 
// 		for (auto& bt : m_jokerSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Jungle_Right_Slot_Jocker.png");
// 
// 		}
// 
// 		for (auto& bt : m_itemSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Jungle_Right_Slot_Item.png");
// 		}
// 
// 		for (auto& bt : m_passiveSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Jungle_Right_Slot_Passive.png");
// 		}
// 
// 		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Right_Slot_Description.png");
// 		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Right_Base_Glow.png");
// 		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Left_Down_Base_Glow.png");
// 		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Left_Base_Glow.png");
// 		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);
// 
// 		m_lastIndex = -1;
// 		Mediator::GetInstance().SetSlotIndex(-1);
// 		ChangeOriginSlot("T_Jungle_Right_Slot_Jocker.png");
// 
// 		m_channel->stop();
// 		m_channel = nullptr;
// 		m_bgm = SoundManager::GetInstance().GetSound("wild1.mp3");
// 		m_soundManager.GetSystem()->getChannel(0, &m_channel);
// 		m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);
// 
// 		break;
// 
// 	case 2: // Space
// 		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
// 
// 		for (auto& bt : m_jokerSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Space_Right_Slot_Jocker.png");
// 		}
// 
// 		for (auto& bt : m_itemSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Space_Right_Slot_Item.png");
// 		}
// 
// 		for (auto& bt : m_passiveSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Space_Right_Slot_Passive.png");
// 		}
// 		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Right_Slot_Description.png");
// 		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Right_Base_Glow.png");
// 		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Left_Down_Base_Glow.png");
// 		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Left_Base_Glow.png");
// 		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);
// 
// 		m_lastIndex = -1;
// 		Mediator::GetInstance().SetSlotIndex(-1);
// 		ChangeOriginSlot("T_Space_Right_Slot_Jocker.png");
// 
// 		m_channel->stop();
// 		m_channel = nullptr;
// 		m_bgm = SoundManager::GetInstance().GetSound("space1.mp3");
// 		m_soundManager.GetSystem()->getChannel(0, &m_channel);
// 		m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);
// 
// 		break;
// 
// 	case 3: // Korea
// 		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
// 
// 		for (auto& bt : m_jokerSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Dancheong_Right_Slot_Jocker.png");
// 		}
// 
// 		for (auto& bt : m_itemSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Dancheong_Right_Slot_Item.png");
// 		}
// 
// 		for (auto& bt : m_passiveSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Dancheong_Right_Slot_Passive.png");
// 		}
// 
// 		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Right_Slot_Description.png");
// 		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Right_Base_Glow.png");
// 		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Left_Down_Base_Glow.png");
// 		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Left_Base_Glow.png");
// 		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);
// 
// 		m_lastIndex = -1;
// 		Mediator::GetInstance().SetSlotIndex(-1);
// 		ChangeOriginSlot("T_Dancheong_Right_Slot_Jocker.png");
// 
// 		m_channel->stop();
// 		m_channel = nullptr;
// 		m_bgm = SoundManager::GetInstance().GetSound("dancheong1.mp3");
// 		m_soundManager.GetSystem()->getChannel(0, &m_channel);
// 		m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);
// 
// 		break;
// 
// 	case 4: // Halloween
// 		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
// 
// 		for (auto& bt : m_jokerSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Halloween_Right_Slot_Jocker.png");
// 		}
// 
// 		for (auto& bt : m_itemSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Halloween_Right_Slot_Item.png");
// 		}
// 
// 		for (auto& bt : m_passiveSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Halloween_Right_Slot_Passive.png");
// 		}
// 		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Right_Slot_Description.png");
// 		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Right_Base_Glow.png");
// 		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Left_Down_Base_Glow.png");
// 		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Left_Base_Glow.png");
// 		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);
// 
// 		m_lastIndex = -1;
// 		Mediator::GetInstance().SetSlotIndex(-1);
// 		ChangeOriginSlot("T_Halloween_Right_Slot_Jocker.png");
// 
// 		m_channel->stop();
// 		m_channel = nullptr;
// 		m_bgm = SoundManager::GetInstance().GetSound("halloween1.mp3");
// 		m_soundManager.GetSystem()->getChannel(0, &m_channel);
// 		m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);
// 
// 		break;
// 
// 	case 5: // Cyber
// 		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
// 
// 		for (auto& bt : m_jokerSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Cyberpunk_Right_Slot_Jocker.png");
// 		}
// 		ChangeOriginSlot("T_Cyberpunk_Right_Slot_Jocker.png");
// 		for (auto& bt : m_itemSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Cyberpunk_Right_Slot_Item.png");
// 		}
// 
// 		for (auto& bt : m_passiveSlot)
// 		{
// 			auto rend = bt->GetComponent<BitmapRender3D>();
// 			//rend->SetShaderType("UIColor");
// 			rend->ChangeTexture("T_Cyberpunk_Right_Passive_Jocker.png");
// 		}
// 		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("Cyberpunk_Description.png");
// 		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("Cyberpunk_Righ.png");
// 		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("Cyberpunk_Left_Down.png");
// 		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("Cyberpunk_Left.png");
// 		m_cyber->GetComponent<BitmapRender3D>()->SetActive(true);
// 
// 		m_lastIndex = -1;
// 		Mediator::GetInstance().SetSlotIndex(-1);
// 
// 		m_channel->stop();
// 		m_channel = nullptr;
// 		m_bgm = SoundManager::GetInstance().GetSound("cyberpunk1.mp3");
// 		m_soundManager.GetSystem()->getChannel(0, &m_channel);
// 		m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);
// 
// 		break;
// 
// 	default: // 기본 테마
// 
// 		Mediator::GetInstance().SetUIColor(XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));
// 
// 		m_desc->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
// 		m_rightUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
// 		m_leftUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
// 		m_leftUpUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
// 		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);
// 
// 		m_lastIndex = -1;
// 		Mediator::GetInstance().SetSlotIndex(-1);
// 		ChangeOriginSlot("T_Standard_Right_Slot_Jocker.png");
// 
// 		m_channel->stop();
// 		m_channel = nullptr;
// 		m_bgm = SoundManager::GetInstance().GetSound("MainBGM.mp3");
// 		m_soundManager.GetSystem()->getChannel(0, &m_channel);
// 		m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);
// 
// 		break;
// 	}
// }
// 
// void TutoScene::ChangeOriginSlot(const std::string& bitmapFile)
// {
// 	for (auto& slot : m_jokerSlot) {
// 		slot->ChangeOriginTexture(bitmapFile);
// 	}
// }
// 
