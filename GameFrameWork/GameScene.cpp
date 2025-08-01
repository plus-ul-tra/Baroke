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
	for (auto& a : m_objectList) {
		a->Update(deltaTime);
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

	unique_ptr<BackGround> backGround = std::make_unique<BackGround>(0, 0, 1920, 1080);
	m_objectList.emplace_back(std::move(backGround));


	auto playerObject = std::make_unique<Player>(
		0.0f, 0.0f, 50.0f, 50.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) // 녹색
	);
	m_player = playerObject.get();
	m_objectList.push_back(std::move(playerObject));

	unique_ptr<Button> button1 = std::make_unique<Button>(-820.0f, 0.0f, 200, 700, "Sample.png", 50);
	m_buttonList.emplace_back(button1.get());
	m_objectList.emplace_back(std::move(button1));

	unique_ptr<JokerButton> button2 = std::make_unique<JokerButton>( 600.0f, 400.0f, 100, 100, "Sample.png", 50);
	button2->SetButtonJoker(Joker, JokerAbility1);
	m_buttonList.emplace_back(button2.get());
	m_objectList.emplace_back(std::move(button2));

	unique_ptr<BoardObject> boardObj = std::make_unique<BoardObject>(POSX, POSY, WIDTH, HEIGHT, CELL, STONEOFFSET, PADDING);
	m_boardObj = boardObj.get();
	m_objectList.emplace_back(std::move(boardObj));

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

