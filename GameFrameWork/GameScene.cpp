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
	unique_ptr<Button> rightUI = std::make_unique<Button>(700.0f, 50.0f, 400.0f, 800.0f, "T_Right_UI.png", 50);
	m_buttonList.emplace_back(rightUI.get());
	m_objectList.emplace_back(std::move(rightUI));

	// 위치에 맞게 조정 필요
	unique_ptr<JokerButton> jokerButton1 = std::make_unique<JokerButton>(600.0f, 400.0f, 100, 100, "JokerPeacock.png", 50);
	jokerButton1->SetButtonJoker(Joker, JokerAbility1);
	jokerButton1->GetComponent<BitmapRender3D>()->SetShaderType("Holo");
	m_buttonList.emplace_back(jokerButton1.get());
	m_UIList.emplace_back(std::move(jokerButton1));

	unique_ptr<JokerButton> jokerButton2 = std::make_unique<JokerButton>(600.0f, 200.0f, 100, 100, "JokerOstrichEgg.png", 50);
	jokerButton2->SetButtonJoker(Joker, JokerAbility2);
	m_buttonList.emplace_back(jokerButton2.get());
	m_UIList.emplace_back(std::move(jokerButton2));

	unique_ptr<JokerButton> jokerButton3 = std::make_unique<JokerButton>(600.0f, 0.0f, 100, 100, "JokerEgg.png", 50);
	jokerButton3->SetButtonJoker(Joker, JokerAbility3);
	jokerButton3->GetComponent<BitmapRender3D>()->SetShaderType("Holo");
	m_buttonList.emplace_back(jokerButton3.get());
	m_UIList.emplace_back(std::move(jokerButton3));

	unique_ptr<JokerButton> jokerButton4 = std::make_unique<JokerButton>(600.0f, -200.0f, 100, 100, "JokerCombination.png", 50);
	jokerButton4->SetButtonJoker(Joker, JokerAbility4);
	m_buttonList.emplace_back(jokerButton4.get());
	m_UIList.emplace_back(std::move(jokerButton4));

	unique_ptr<JokerButton> jokerButton5 = std::make_unique<JokerButton>(600.0f, -400.0f, 100, 100, "JokerBite.png", 50);
	jokerButton5->SetButtonJoker(Joker, JokerAbility5);
	jokerButton5->GetComponent<BitmapRender3D>()->SetShaderType("Holo");
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
		std::cout << "stage clear >> move to shop" << std::endl;
		m_money += 3 + (m_stageNo - 1);
		std::cout << "money : " <<m_money << std::endl;

		StartStage();
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

	SetUIJokerButton();

	StartStage();
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
			std::cout << "Escape Command Received: Pausing Game" << std::endl;
			// 추후 일시정지/재개 로직 여기에
		};

	m_commandMap["F1"] = [this]()
		{
			std::cout << "F1 Command Received: Changing to Title Scene" << std::endl;
			SceneManager::GetInstance().ChangeScene(std::string("Title"));
		};

	m_commandMap["F2"] = [this]()
		{
			SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
		};

	m_commandMap["F3"] = [this]()
		{
			m_board.PlaceRandomStones(3);
		};

	m_commandMap["F4"] = [this]()
		{
			m_board.ResetStone();
		};

	m_commandMap["F5"] = [this]()
		{
			SceneManager::GetInstance().ChangePostProcessing("PassThrough");
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
	if (ev.type == MouseType::LDown)
	{
		if (m_BlackStone <= m_board.GetStoneTypeAmount(Black)) return;
		std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
		m_board.InputBasedGameLoop(ev.pos);
		std::cout << "Black Stone Count : " << m_board.GetStoneTypeAmount(Black) << std::endl;
	}

	else if (ev.type == MouseType::RDown)
	{
		std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
		m_board.SetStoneType(Joker);
		m_board.SetStoneAbility(JokerAbility1);
		m_board.InputBasedGameLoop(ev.pos);
		std::cout << "Joker Stone Count : " << m_board.GetStoneTypeAmount(Joker) << std::endl;
	}

	for (auto& button : m_buttonList)
	{
		button->CheckInput(ev);
	}
}

