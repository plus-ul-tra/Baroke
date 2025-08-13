#include "cpch.h"
#include "GameScene.h"

#define BOARD_SIZE 15
#define PADDING 106
#define POSX 0
#define POSY 0
#define WIDTH 970
#define HEIGHT 970
#define CELL 54  // 0 기준 54 pxs
#define STONEOFFSET 10
using namespace std;

void GameScene::initGame()
{
	m_stageNo = 0;						// 스테이지 단계 초기화
	SetUIButton();						// 버튼들 초기화
	ChangeThema(0);						// 초기 테마 설정

	SetHintpool();						// 힌트 풀 초기화
	m_board.GetPlayer().initGame();		// 플레이어 초기화
	m_shopStones.clear();				// 상점 돌 초기화
	m_shopItems.clear();				// 상점 아이템 초기화

	StartStage();						// 스테이지 시작

	InitShop();							// 상점 초기화
	m_board.ResetStagePlaced(); // 조커 착수 트래킹 리셋
}

void GameScene::SetUIButton()
{
	m_shopBuyStoneButton = make_unique<ShopBuyStoneButton>(-272.0f, -272.0f, 100, 100, "T_Blackstone.png");
	m_shopBuyStoneButton->SetShowAndActive(false);
	m_buttonList.emplace_back(m_shopBuyStoneButton.get());
	m_notUniqueObjectList.emplace_back(m_shopBuyStoneButton.get());

	unique_ptr<Text> buyStonePrice = std::make_unique<Text>(-298.0f, -324.0f, 100.0f, 50.0f, 0.75);
	buyStonePrice->GetComponent<UIText>()->SetText(000000);
	buyStonePrice->GetComponent<UIText>()->SetActive(false);
	m_buyStonePriceText = buyStonePrice.get();
	m_textList.emplace_back(buyStonePrice.get());
	m_useless.emplace_back(move(buyStonePrice));
	//---------------------------------------------------------------------------------------------------------
	m_shopShopRerollButton = make_unique<ShopRerollButton>(-3.0f, -275.0f, 0, 0, "T_Store_Reset.png");
	m_shopShopRerollButton->SetShowAndActive(false);
	m_buttonList.emplace_back(m_shopShopRerollButton.get());
	m_notUniqueObjectList.emplace_back(m_shopShopRerollButton.get());

	unique_ptr<Text> buyRerollPrice = std::make_unique<Text>(-53.0f, -340.0f, 150.0f, 75.0f, 0.75);
	buyRerollPrice->GetComponent<UIText>()->SetText(000000);
	buyRerollPrice->GetComponent<UIText>()->SetActive(false);
	m_buyRerollPriceText = buyRerollPrice.get();
	m_textList.emplace_back(buyRerollPrice.get());
	m_useless.emplace_back(move(buyRerollPrice));

	m_shopExitButton = make_unique<ShopEndButton>(270.0f, -287.0f, 145, 237, "T_Store_Next_Stage.png");
	m_shopExitButton->SetShowAndActive(false);
	m_buttonList.emplace_back(m_shopExitButton.get());
	m_notUniqueObjectList.emplace_back(m_shopExitButton.get());

	//1째
	for (int i = 0; i < 3; i++)
	{
		m_shopJokerButtons[i] = make_unique<ShopJokerButton>(-272.0f + (i * 270.0f), 270.0f, 100, 100, "Sample.png");
		m_shopJokerButtons[i]->SetShowAndActive(false);
		m_buttonList.emplace_back(m_shopJokerButtons[i].get());
		m_notUniqueObjectList.emplace_back(m_shopJokerButtons[i].get());

		unique_ptr<Text> shopJokerText = std::make_unique<Text>(-328.0f + (i * 270.0f), 220.0f, 150.0f, 75.0f, 0.75);
		shopJokerText->GetComponent<UIText>()->SetText(000000);
		shopJokerText->GetComponent<UIText>()->SetActive(false);
		m_shopJokerTexts[i] = shopJokerText.get();
		m_textList.emplace_back(shopJokerText.get());
		m_useless.emplace_back(move(shopJokerText));
	}

	// 2째
	for (int i = 0; i < 3; i++)
	{
		m_shopJokerButtons[3 + i] = make_unique<ShopJokerButton>(-272.0f + (i * 270.0f), 0.0f, 75, 75, "Sample.png");
		m_shopJokerButtons[3 + i]->SetShowAndActive(false);
		m_buttonList.emplace_back(m_shopJokerButtons[3 + i].get());
		m_notUniqueObjectList.emplace_back(m_shopJokerButtons[3 + i].get());

		unique_ptr<Text> shopJokerText = std::make_unique<Text>(-328.0f + (i * 270.0f), -50.0f, 150.0f, 75.0f, 0.75);
		shopJokerText->GetComponent<UIText>()->SetText(000000);
		shopJokerText->GetComponent<UIText>()->SetActive(false);
		m_shopJokerTexts[3 + i] = shopJokerText.get();
		m_textList.emplace_back(shopJokerText.get());
		m_useless.emplace_back(move(shopJokerText));
	}

	//--------------------------UI일반----------------------------------
	unique_ptr<Button> rightUI = std::make_unique<Button>(700.0f, 0.0f, 427, 969, "T_Standard_Right_Base_Glow.png");
	m_notUniqueObjectList.emplace_back(rightUI.get());
	//m_normalUI.emplace_back(move(rightUI));
	rightUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_rightUI = move(rightUI);

	unique_ptr<Button> desc = std::make_unique<Button>(700.0f, 0.0f, 386, 734, "T_Standard_Right_Slot_Description.png");
	m_notUniqueObjectList.emplace_back(desc.get());
	//m_normalUI.emplace_back(move(desc));
	desc->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
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
	//--------------------------흑돌 흰돌 UI----------------------------------
	unique_ptr<Button> BlackUI = std::make_unique<Button>(-800.0f, 120.0f, 170, 170, "T_Blackstone.png");
	m_notUniqueObjectList.emplace_back(BlackUI.get());
	//leftUpUI->AddComponent<UIText>(-680.0f, 440.0f, 100.0f, 100.0f, 2);
/*	leftUpUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");*/
	m_BlackUI = move(BlackUI);

	unique_ptr<Button> WhiteUI = std::make_unique<Button>(-640.0f, 120.0f, 170, 170, "T_whitestone.png");
	m_notUniqueObjectList.emplace_back(WhiteUI.get());
	//leftUpUI->AddComponent<UIText>(-680.0f, 440.0f, 100.0f, 100.0f, 2);
/*	leftUpUI->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");*/
	m_WhiteUI = move(WhiteUI);


	//--------------------동적 Text---------------------
	unique_ptr<Text> textStage = std::make_unique<Text>(-680.0f, 435.0f, 100.0f, 100.0f, 2);
	textStage->GetComponent<UIText>()->SetText(10);
	m_stageText = textStage.get();
	m_textList.emplace_back(textStage.get());
	m_useless.emplace_back(move(textStage)); //렌더용

	unique_ptr<Text> textScore = std::make_unique<Text>(-850.0f, -200.0f, 260.0f, 100.0f, 2);
	textScore->GetComponent<UIText>()->SetText(000000);
	m_scoreText = textScore.get();
	m_textList.emplace_back(textScore.get());
	m_useless.emplace_back(move(textScore));


	unique_ptr<Text> popupMsg = std::make_unique<Text>(-1000.f, 300.f, 2000.0f, 100.0f, 1);
	popupMsg->GetComponent<UIText>()->SetText(L"No more moves available");
	popupMsg->GetComponent<UIText>()->SetActive(false);
	m_popupText = popupMsg.get();
	m_textList.emplace_back(popupMsg.get());
	m_useless.emplace_back(move(popupMsg));

	unique_ptr<Text> BlackCount = std::make_unique<Text>(-900.0f, -4.0f, 200.0f, 100.0f,0 );
	BlackCount->GetComponent<UIText>()->SetText(12);
	m_BlackText = BlackCount.get();
	m_textList.emplace_back(BlackCount.get());
	m_useless.emplace_back(move(BlackCount)); //렌더용

	unique_ptr<Text> WhiteCount = std::make_unique<Text>(-740.0f, -4.0f, 200.0f, 100.0f, 0 );
	WhiteCount->GetComponent<UIText>()->SetText(0);
	m_WhiteText = WhiteCount.get();
	m_textList.emplace_back(WhiteCount.get());
	m_useless.emplace_back(move(WhiteCount)); //렌더용

	unique_ptr<Text> WaxCount = std::make_unique<Text>(-820.0f, -28.0f, 200.0f, 100.0f, 0);
	WaxCount->GetComponent<UIText>()->SetText(0);
	m_WaxText = WaxCount.get();
	m_textList.emplace_back(WaxCount.get());
	m_useless.emplace_back(move(WaxCount)); //렌더용

	//-- unchanged----------------------
	// 장식품
	unique_ptr<Button> stageText = std::make_unique<Button>(-760.0f, 388.0f, 167, 67, "T_Common_Left_Stage.png");
	m_notUniqueObjectList.emplace_back(stageText.get());
	stageText->GetComponent<BitmapRender3D>()->SetShaderType("Holo");
	m_useless.emplace_back(move(stageText));

	unique_ptr<Button> leftLine = std::make_unique<Button>(-720.0f, 0.0f, 340, 60, "T_Common_Left_Down_Score_Guideline.png");
	m_notUniqueObjectList.emplace_back(leftLine.get());
	leftLine->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_useless.emplace_back(move(leftLine));

	unique_ptr<Button> scoreText = std::make_unique<Button>(-720.0f, -150.0f, 145, 40, "T_Common_Left_Down_Score.png");
	m_notUniqueObjectList.emplace_back(scoreText.get());
	scoreText->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_useless.emplace_back(move(scoreText));

	unique_ptr<Button> scoreFiled = std::make_unique<Button>(-720.0f, -250.0f, 204, 97, "T_Common_Left_Down_Score_Field.png");
	m_notUniqueObjectList.emplace_back(scoreFiled.get());
	scoreFiled->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_useless.emplace_back(move(scoreFiled));

	unique_ptr<ResetStageButton> settingText = std::make_unique<ResetStageButton>(-720.0f, -330.0f, 93, 38, "T_Common_1Reset.png");
	m_buttonList.emplace_back(settingText.get());
	m_notUniqueObjectList.emplace_back(settingText.get());
	m_resetStageButton = settingText.get();
	//settingText->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_useless.emplace_back(move(settingText));

	unique_ptr<SceneChangeButton> exitText = std::make_unique<SceneChangeButton>(-720.0f, -390.0f, 115, 32, "T_Common_Left_Down_Exit.png", "Ending", buttonType::GameToEnding);
	m_buttonList.emplace_back(exitText.get());
	m_notUniqueObjectList.emplace_back(exitText.get());
	exitText->GetComponent<BitmapRender3D>()->SetShaderType("UIHolo");
	m_useless.emplace_back(move(exitText));

	unique_ptr<Button> cyberAlpha = std::make_unique<Button>(0.0f, 0.0f, 900, 810, "CyberAlpha.png");
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
	jokerSlot1->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png",0);
	m_buttonList.emplace_back(jokerSlot1.get());
	m_notUniqueObjectList.emplace_back(jokerSlot1.get());
	m_jokerSlot.emplace_back(move(jokerSlot1));
	

	unique_ptr<Button> jokerSlot2 = std::make_unique<Button>(617.0f, 171.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot2->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png",1);
	m_buttonList.emplace_back(jokerSlot2.get());
	m_notUniqueObjectList.emplace_back(jokerSlot2.get());
	m_jokerSlot.emplace_back(move(jokerSlot2));
	

	unique_ptr<Button> jokerSlot3 = std::make_unique<Button>(617.0f, 1.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot3->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png",2);
	m_buttonList.emplace_back(jokerSlot3.get());
	m_notUniqueObjectList.emplace_back(jokerSlot3.get());
	m_jokerSlot.emplace_back(move(jokerSlot3));
	

	unique_ptr<Button> jokerSlot4 = std::make_unique<Button>(617.0f, -172.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot4->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png",3);
	m_buttonList.emplace_back(jokerSlot4.get());
	m_notUniqueObjectList.emplace_back(jokerSlot4.get());
	m_jokerSlot.emplace_back(move(jokerSlot4));
	

	unique_ptr<Button> jokerSlot5 = std::make_unique<Button>(617.0f, -342.0f, 177, 175, "T_Standard_Right_Slot_Jocker.png");
	jokerSlot5->RegistClickedTexture("T_Standard_Right_Slot_Jocker_Glow.png",4);
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
	jokerButton1->SetButtonJoker(Black, jokerWaxseal);
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

	// 액티브 조커
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



void GameScene::StartStage()
{
	m_stageNo++;

	int spawn = 3 + (m_stageNo * 2);

	m_resetStageButton->SetIsActive(true);
	m_resetStageButton->SetWhiteStonePos(m_board.PlaceRandomStones(spawn));
	m_whiteLeft = m_board.GetStoneTypeAmount(White);
	m_board.m_playerInfo.m_rerollCount = 0; // 리롤 횟수 초기화

	if (m_stageText)
		if (auto ui = m_stageText->GetComponent<UIText>())
			ui->SetText(m_stageNo);

	m_soundManager.PlaySoundOnce("stonePlace.wav");

	std::cout << "Stage " << m_stageNo/* << " start, Spawn White Conut : " << spawn << std::endl*/;

}

void GameScene::CheckStageClear()
{
	if (m_board.GetStoneTypeAmount(White))
	{
		// 조커 조건은 어떤 식으로?
		if (m_board.m_playerInfo.GetBlackCount() == m_board.GetStoneTypeAmount(Black))
		{
			bool isAvailableJoker = false;
			for (auto& jokerButton : m_jokerButtons)
			{
				if (!jokerButton->IsActive()) continue;
				isAvailableJoker = true;
			}
			if (!isAvailableJoker)
			{
				m_gameState = GameState::Ending;
				if (m_gameStateDelayElapsed < m_gameStateDelay) return;
				SceneManager::GetInstance().SetExit(true, "Ending");
			}
		}
		else m_gameStateDelayElapsed = 0.0f;
	}
	else  // 스테이지 클리어
	{

		SceneManager::GetInstance().ChangePostProcessing("CRTFilter");

		if (m_gameState == GameState::Stage) m_gameState = GameState::ShopEnter;
    
		if (m_gameStateDelayElapsed < m_gameStateDelay) return;    // 잠시 꺼둠 최종빌드때 다시 켜야댐

		if (m_gameState == GameState::ShopEnter)
		{
			m_resetStageButton->SetIsActive(false);
			m_gameState = GameState::Shop;
			ShopStage();

			m_shopBuyStoneButton->SetShowAndActive(true);
			m_buyStonePriceText->GetComponent<UIText>()->SetActive(true);
			m_shopShopRerollButton->SetShowAndActive(true);
			m_buyRerollPriceText->GetComponent<UIText>()->SetActive(true);
			m_shopExitButton->SetShowAndActive(true);

			m_channel->stop();
			m_channel = nullptr;
			m_bgm = nullptr;
			m_bgm = m_soundManager.GetSound("ShopBGM.mp3");
			m_bgm->setMode(FMOD_LOOP_NORMAL);

			m_soundManager.ReleaseChannelGroup();
			if (m_bgm) SoundManager::GetInstance().GetSystem()->playSound(m_bgm, m_soundManager.GetChannelGroup(), false, &m_channel);
			m_channel->setVolume(BGM_VOLUME);
		}
		if (m_gameState == GameState::Shop && m_shopBuyStoneButton && m_shopShopRerollButton && m_shopExitButton)
		{
			if (m_shopShopRerollButton->IsReroll()) ShopStage(); // 상점 리롤
			if (m_shopExitButton->IsEndButtonPressed())
			{
				m_gameState = GameState::ShopExit;

				m_shopBuyStoneButton->SetShowAndActive(false);
				m_buyStonePriceText->GetComponent<UIText>()->SetActive(false);
				m_shopShopRerollButton->SetShowAndActive(false);
				m_buyRerollPriceText->GetComponent<UIText>()->SetActive(false);
				m_shopExitButton->SetShowAndActive(false);

				for (auto& jokerButton : m_shopJokerButtons) jokerButton->SetShowAndActive(false);
				for (auto& shopJokerText : m_shopJokerTexts)
				{
					shopJokerText->GetComponent<UIText>()->SetActive(false);
					shopJokerText->GetComponent<UIText>()->SetText(000000);
				}

				m_gameStateDelayElapsed = 0.0f;

				if (m_stageNo % 2)
				{
					ChangeThema((m_stageNo / 2 + 1) % 6);
					m_boardObj->ChangeTheme((m_stageNo / 2 + 1) % 6);
				}
				else
				{
					ChangeThema(0);
					m_boardObj->ChangeTheme(0);
				}
			}
		}

		m_board.ResetStone();
		m_board.ResetStagePlaced(); // 조커 착수 트래킹 리셋
		m_board.m_playerInfo.ResetRound();

		if (m_gameState == GameState::ShopExit && m_gameStateDelayElapsed > m_gameStateDelay) m_gameState = GameState::Stage;
		if (m_gameState == GameState::Stage) StartStage();
	}
}
void GameScene::CheckSlot()
{
	int index = Mediator::GetInstance().GetSlotIndex();
	if (index != -1 && m_lastIndex !=index) {
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

void GameScene::ModeCheck()
{
	m_uiMode = m_board.GetMode();
	//std::cout << m_uiMode << std::endl;

	if (m_uiMode==UIMode::Sacrifice &&m_board.checkSacrificeSuccess())
	{
		m_board.SetMode(UIMode::BeforeUseAbility);
		std::cout << "Sacrifice clear" << std::endl;
	}
	
	if (m_uiMode == UIMode::BeforeUseAbility&&m_board.checkBeforeAbSuccess())
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

void GameScene::CRTAccess()
{
	if (m_uiMode == UIMode::Normal) { SceneManager::GetInstance().ChangePostProcessing("CRTFilter"); }
	else if (m_uiMode == UIMode::Sacrifice){ SceneManager::GetInstance().ChangePostProcessing("CRTRed"); }
	else if (m_uiMode == UIMode::BeforeUseAbility) { SceneManager::GetInstance().ChangePostProcessing("CRTGreen"); }
	else if (m_uiMode == UIMode::UseAbility) { SceneManager::GetInstance().ChangePostProcessing("CRTFilter"); }

}

void GameScene::InitShop()
{
	m_gameState = GameState::Stage;
	for (auto& jokers : m_jokerInfoMap)
	{
		if (!jokers.second.inShop) continue; // 상점에 없는 조커는 제외
		if (jokers.second.isStone) m_shopStones.emplace_back(jokers.first, jokers.second);
		else m_shopItems.emplace_back(jokers.first, jokers.second);
	}
}

void GameScene::ShopStage()
{
	m_board.ResetStone();
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> dist(0, 99);

	for (auto& shopJokerButton : m_shopJokerButtons) { // 이전 상점 정보 초기화
		shopJokerButton->SetShowAndActive(false);
		shopJokerButton->SetButtonJoker(JokerStoneInfo{}, StoneAbility::None);
	}
	for (auto& shopJokerText : m_shopJokerTexts)
	{
		shopJokerText->GetComponent<UIText>()->SetActive(false);
		shopJokerText->GetComponent<UIText>()->SetText(000000);
	}
	

	for (int i = 0; i < 3; i++)
	{
		int random = m_shopRng[i];
		if (i == 0) random += 20;
		if (dist(rng) < random)
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
			m_shopJokerTexts[i]->GetComponent<UIText>()->SetActive(true);
		}
	}
	for (int i = 0; i < 3; i++)
	{
		if (dist(rng) < m_shopRng2[i])
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
			m_shopJokerTexts[3 + i]->GetComponent<UIText>()->SetActive(true);
		}
	}
}

void GameScene::SyncPlacementHintsToPool()
{
	const auto& hints = m_board.GetHints(); 

	for (auto& h : m_hintPool) h->Show(false);

	const size_t n = std::min(hints.size(), m_hintPool.size());
	for (size_t i = 0; i < n; ++i)
	{
		POINT screen = m_board.BoardToScreenPosition(hints[i]);

		float sx = static_cast<float>(screen.x);
		float sy = static_cast<float>(screen.y);


		m_hintPool[i]->SetScreenPos(sx + CELL/2-1, sy + CELL / 2);
		m_hintPool[i]->Show(true);
	}
}


void GameScene::SetHintpool()
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
	m_filterElsapsedTime += deltaTime;

	if (m_boardObj)
	{
		m_boardObj->BoardSync();
		m_boardObj->Update(deltaTime);
	}
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


	if (m_buyStonePriceText)
		if (auto price = m_buyStonePriceText->GetComponent<UIText>())
			price->SetText(m_board.m_playerInfo.m_blackStoneUpgrade * 2 + 1);

	if (m_buyRerollPriceText)
		if (auto price = m_buyRerollPriceText->GetComponent<UIText>())
			price->SetText(m_board.m_playerInfo.m_rerollCount);

	if (m_shopJokerTexts)
		for (int i = 0; i < 6; i++)
			if (auto text = m_shopJokerTexts[i]->GetComponent<UIText>())
			{
				text->SetText(m_jokerInfoMap[m_shopJokerButtons[i]->GetJokerAbility()].costWhite);

				if (m_shopJokerButtons[i]->IsActive()) text->SetActive(true);
				else text->SetActive(false);
			}

	if (m_scoreText)
		if (auto score = m_scoreText->GetComponent<UIText>())
			score->SetText(m_board.m_playerInfo.m_score);

	if (m_BlackText)
		if (auto black = m_BlackText->GetComponent<UIText>())
			black->SetText(m_board.m_playerInfo.GetBlackCount()-m_board.CountStones(Black));

	if (m_WhiteText)
		if (auto white = m_WhiteText->GetComponent<UIText>())
			white->SetText(m_board.m_playerInfo.m_money);

	if (m_WaxText)
		if (auto wax = m_WaxText->GetComponent<UIText>())
			wax->SetText(m_board.m_playerInfo.m_waxMoney);


// 	if (m_board.GetPlayer().GetBlackCount() <= m_board.GetStoneTypeAmount(Black))
// 	{
// 		m_popupText->GetComponent<UIText>()->SetActive(true);
// 	}
// 	else
// 	{
// 		m_popupText->GetComponent<UIText>()->SetActive(false);
// 	}

	if (!DirectX::XMVector3Equal(m_moveDir, DirectX::XMVectorZero()) && m_player)
	{
		DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(m_moveDir);
		m_player->Move(dir, deltaTime);
	}

	m_moveDir = DirectX::XMVectorZero();

	
	m_gameStateDelayElapsed += deltaTime;

	CheckSlot();
	ModeCheck();
	CheckStageClear();

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
}

void GameScene::LateUpdate(double deltaTime)
{
}

void GameScene::OnEnter()
{
	std::cout << "Game1 Scene OnEnter" << std::endl;

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

	unique_ptr<BoardObject> boardObj = make_unique<BoardObject>(POSX, POSY, WIDTH, HEIGHT, CELL, STONEOFFSET, PADDING);
	m_boardObjRender = boardObj.get();
	m_boardObj = move(boardObj);

	m_lastIndex = -1;

	// 사운드
	m_channel->stop();
	m_bgm = nullptr;
	m_channel = nullptr;
	m_bgm = SoundManager::GetInstance().GetSound("MainBGM.mp3");
	m_bgm->setMode(FMOD_LOOP_NORMAL);
	m_soundManager.GetSystem()->playSound(m_bgm, m_soundManager.GetChannelGroup(), false, &m_channel);
	m_channel->setVolume(BGM_VOLUME);


	initGame();
}

void GameScene::OnLeave()
{
	std::cout << "Game1 Scene Left" << std::endl;
	Reset();
	m_useless.clear();
	m_jokerButtons.clear();
	m_jokerSlot.clear();
	m_itemSlot.clear();
	m_passiveSlot.clear();
	m_hintPool.clear();
	m_lastIndex = -1;

	m_board.ResetStone();
	m_boardObj = nullptr;
	m_boardObjRender = nullptr;

	// 사운드 초기화
	m_channel->stop();
	m_bgm = nullptr;
	m_channel = nullptr;
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
			m_board.ClearHints();
			m_board.ExitMode();
			SyncPlacementHintsToPool();
			SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
			// 추후 일시정지/재개 로직 여기에
		};

	m_commandMap["F1"] = [this]()
		{
			
			SceneManager::GetInstance().ChangePostProcessing("PassThrough");
			m_filterElsapsedTime = -5.0f;

		};

	m_commandMap["F2"] = [this]()
		{
			SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
			m_filterElsapsedTime = 0.0f;
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
	if (m_gameState == GameState::Ending) return; // 엔딩 모드
	if (m_filterElsapsedTime < 0.0f) return; // 필터가 적용 중이라면 정지
	for (auto& button : m_buttonList)
	{
		button->CheckInput(ev);
	}
	if (m_gameState == GameState::ShopEnter) return; // 상점 진입 모드
	if (m_gameState == GameState::Shop) return; // 상점 모드
	if (m_gameState == GameState::ShopExit) return; // 상점 종료 모드

	if (m_uiMode == UIMode::Normal)
	{
		if (ev.type == MouseType::LDown)
		{
			if (m_board.GetPlayer().GetBlackCount() <= m_board.GetStoneTypeAmount(Black)) return;
			m_board.InputBasedGameLoop(ev.pos);
			std::cout << "Place Black Stone : " << m_board.GetStoneTypeAmount(Black) << " / " << m_board.GetPlayer().GetBlackCount() << std::endl;
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

// 	else if (m_uiMode ==UIMode::UseAbility)  //능력 사용 모드
// 	{
// 		if (ev.type == MouseType::LDown) {
// 			m_board.SetStoneType(m_board.GetStoneType());
// 			m_board.SetStoneAbility(m_board.GetStoneAbility());
// 			if (m_board.InputBasedGameLoop(ev.pos))
// 			{
// 				m_board.ExitMode();		// 능력 사용 후 다시 초기화
// 			}
// 			std::cout << "Place Black Stone : " << m_board.GetStoneTypeAmount(Black) << " / " << m_board.GetPlayer().GetBlackCount() << std::endl;
// 		}
// 
// 	}


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

void GameScene::ChangeThema(int thema)
{
	switch (thema)
	{
	case 1: // Forest
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_jokerSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Jungle_Right_Slot_Jocker.png");
			
		}

		for (auto& bt : m_itemSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Jungle_Right_Slot_Item.png");
		}

		for (auto& bt : m_passiveSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Jungle_Right_Slot_Passive.png");
		}

		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Right_Slot_Description.png");
		//m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Right_Base_Glow.png");
		//m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Left_Down_Base_Glow.png");
		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Left_Base_Glow.png");


		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Right_Slot_Description.png");
		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Right_Base.png");
		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Left_Down_Base.png");
		//m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Jungle_Left_Base.png");


		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);

		m_lastIndex = -1;
		Mediator::GetInstance().SetSlotIndex(-1);
		ChangeOriginSlot("T_Jungle_Right_Slot_Jocker.png");

		m_channel->stop();
		m_channel = nullptr;
		m_bgm = SoundManager::GetInstance().GetSound("wild1.mp3");
		m_bgm->setMode(FMOD_LOOP_NORMAL);

		m_soundManager.ReleaseChannelGroup();
		m_soundManager.GetSystem()->playSound(m_bgm, m_soundManager.GetChannelGroup(), false, &m_channel);
		m_channel->setVolume(BGM_VOLUME);

		break;

	case 2: // Space
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_jokerSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Space_Right_Slot_Jocker.png");
		}

		for (auto& bt : m_itemSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Space_Right_Slot_Item.png");
		}

		for (auto& bt : m_passiveSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Space_Right_Slot_Passive.png");
		}
		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Right_Slot_Description.png");
		//m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Right_Base_Glow.png");
		//m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Left_Down_Base_Glow.png");
		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Left_Base_Glow.png");


		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Right_Slot_Description.png");
		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Right_Base.png");
		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Left_Down_Base.png");
		//m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Space_Left_Base.png");

		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);

		m_lastIndex = -1;
		Mediator::GetInstance().SetSlotIndex(-1);
		ChangeOriginSlot("T_Space_Right_Slot_Jocker.png");

		m_channel->stop();
		m_channel = nullptr;
		m_bgm = SoundManager::GetInstance().GetSound("space1.mp3");
		m_bgm->setMode(FMOD_LOOP_NORMAL);

		m_soundManager.ReleaseChannelGroup();
		m_soundManager.GetSystem()->playSound(m_bgm, m_soundManager.GetChannelGroup(), false, &m_channel);
		m_channel->setVolume(BGM_VOLUME);

		break;

	case 3: // Korea
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_jokerSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Dancheong_Right_Slot_Jocker.png");
		}

		for (auto& bt : m_itemSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Dancheong_Right_Slot_Item.png");
		}

		for (auto& bt : m_passiveSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Dancheong_Right_Slot_Passive.png");
		}

		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Right_Slot_Description.png");
		//m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Right_Base_Glow.png");
		//m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Left_Down_Base_Glow.png");
		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Left_Base_Glow.png");


		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Right_Base.png");
		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Left_Down_Base.png");
		//m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Dancheong_Left_Base.png");

		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);

		m_lastIndex = -1;
		Mediator::GetInstance().SetSlotIndex(-1);
		ChangeOriginSlot("T_Dancheong_Right_Slot_Jocker.png");

		m_channel->stop();
		m_channel = nullptr;
		m_bgm = SoundManager::GetInstance().GetSound("dancheong1.mp3");
		m_bgm->setMode(FMOD_LOOP_NORMAL);

		m_soundManager.ReleaseChannelGroup();
		m_soundManager.GetSystem()->playSound(m_bgm, m_soundManager.GetChannelGroup(), false, &m_channel);
		m_channel->setVolume(BGM_VOLUME);

		break;

	case 4: // Halloween
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_jokerSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Halloween_Right_Slot_Jocker.png");
		}

		for (auto& bt : m_itemSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Halloween_Right_Slot_Item.png");
		}

		for (auto& bt : m_passiveSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Halloween_Right_Slot_Passive.png");
		}
		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Right_Slot_Description.png");
		//m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Right_Base_Glow.png");
		//m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Left_Down_Base_Glow.png");
		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Left_Base_Glow.png");


		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Right_Base.png");
		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Left_Down_Base.png");
		//m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Halloween_Left_Base.png");
		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);

		m_lastIndex = -1;
		Mediator::GetInstance().SetSlotIndex(-1);
		ChangeOriginSlot("T_Halloween_Right_Slot_Jocker.png");

		m_channel->stop();
		m_channel = nullptr;
		m_bgm = SoundManager::GetInstance().GetSound("halloween1.mp3");
		m_bgm->setMode(FMOD_LOOP_NORMAL);

		m_soundManager.ReleaseChannelGroup();
		m_soundManager.GetSystem()->playSound(m_bgm, m_soundManager.GetChannelGroup(), false, &m_channel);
		m_channel->setVolume(BGM_VOLUME);

		break;

	case 5: // Cyber
		Mediator::GetInstance().SetUIColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		for (auto& bt : m_jokerSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Cyberpunk_Right_Slot_Jocker.png");
		}
		ChangeOriginSlot("T_Cyberpunk_Right_Slot_Jocker.png");
		for (auto& bt : m_itemSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Cyberpunk_Right_Slot_Item.png");
		}

		for (auto& bt : m_passiveSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Cyberpunk_Right_Passive_Jocker.png");
		}
		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("Cyberpunk_Description.png");
		//m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("Cyberpunk_Righ.png");
		//m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("Cyberpunk_Left_Down.png");
		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("Cyberpunk_Left.png");


		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Cyberpunk_Right_Base.png");
		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Cyberpunk_Left_Down_Base.png");
		//m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Cyberpunk_Left_Base.png");
		m_cyber->GetComponent<BitmapRender3D>()->SetActive(true);

		m_lastIndex = -1;
		Mediator::GetInstance().SetSlotIndex(-1);

		m_channel->stop();
		m_channel = nullptr;
		m_bgm = SoundManager::GetInstance().GetSound("cyberpunk1.mp3");
		m_bgm->setMode(FMOD_LOOP_NORMAL);

		m_soundManager.ReleaseChannelGroup();
		m_soundManager.GetSystem()->playSound(m_bgm, m_soundManager.GetChannelGroup(), false, &m_channel);
		m_channel->setVolume(BGM_VOLUME);

		break;

	default: // 기본 테마

		Mediator::GetInstance().SetUIColor(XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

// 		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Standard_Right_Slot_Description.png");
 		//m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Standard_Right_Base_Glow.png");
// 		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Standard_Left_Down_Base_Glow.png");
 		m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Standard_Left_Base_Glow.png");
// 		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);

		m_desc->GetComponent<BitmapRender3D>()->ChangeTexture("T_Standard_Right_Slot_Description.png");
		m_rightUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Standard_Right_Base.png");
		m_leftUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Standard_Left_Down_Base.png");
		//m_leftUpUI->GetComponent<BitmapRender3D>()->ChangeTexture("T_Standard_Left_Base.png");
		m_cyber->GetComponent<BitmapRender3D>()->SetActive(false);

		m_lastIndex = -1;
		Mediator::GetInstance().SetSlotIndex(-1);
		ChangeOriginSlot("T_Standard_Right_Slot_Jocker.png");

		m_channel->stop();
		m_channel = nullptr;
		m_bgm = SoundManager::GetInstance().GetSound("MainBGM.mp3");
		m_bgm->setMode(FMOD_LOOP_NORMAL);

		m_soundManager.ReleaseChannelGroup();
		m_soundManager.GetSystem()->playSound(m_bgm, m_soundManager.GetChannelGroup(), false, &m_channel);
		m_channel->setVolume(BGM_VOLUME);

		for (auto& bt : m_jokerSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Standard_Right_Slot_Jocker.png");
		}
		for (auto& bt : m_itemSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Standard_Right_Slot_Item.png");
		}

		for (auto& bt : m_passiveSlot)
		{
			auto rend = bt->GetComponent<BitmapRender3D>();
			//rend->SetShaderType("UIColor");
			rend->ChangeTexture("T_Standard_Right_Slot_Passive.png");
		}

		break;
	}
}

void GameScene::ChangeOriginSlot(const std::string& bitmapFile)
{
	for (auto& slot : m_jokerSlot) {
		slot->ChangeOriginTexture(bitmapFile);
	}
}

