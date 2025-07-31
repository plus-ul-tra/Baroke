#include "cpch.h"
#include "GameScene.h"


#define BOARD_SIZE 15
#define PADDING 72
#define POSX 510
#define POSY 90
#define WIDTH 900
#define HEIGHT 900
#define STONEOFFSET 10  // 0 기준 60 px


#define STONEOFFSET 10


void GameScene::Initialize()
{
	std::cout << "Game Scene Init" << std::endl;
	KeyCommandMapping();
	//m_board = CreateBoard(BOARD_SIZE);

	//auto boardObj = std::make_unique<BoardObject>(
	//	m_board.get(), POSX, POSY, WIDTH, HEIGHT, PADDING, STONEOFFSET);
	//m_boardObj = boardObj.get();
	//m_objectList.emplace_back(std::move(boardObj));

	//m_board->PlaceStone(0, 0, { StoneColor::White ,StoneAbility::None });
	//m_board->PlaceStone(0, 1, { StoneColor::White ,StoneAbility::None });
	//m_board->PlaceStone(0, 2, { StoneColor::White ,StoneAbility::None });
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
	auto playerObject = std::make_unique<Player>(
		600.0f, 300.0f, 50.0f, 50.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) // 녹색
	);
	m_player = playerObject.get();
	m_objectList.push_back(std::move(playerObject));
	std::cout << m_objectList.size() << std::endl;

	// 버튼 생성
	unique_ptr<Button> button1 = std::make_unique<Button>(30.0f, 30.0f, 200, 200, "Sample.png", 50);
	m_buttonList.emplace_back(button1.get());
	m_objectList.emplace_back(std::move(button1));

	unique_ptr<BoardObject> boardObj = std::make_unique<BoardObject>(POSX, POSY, WIDTH, HEIGHT, PADDING);
	m_boardObj = boardObj.get();
	m_objectList.emplace_back(std::move(boardObj));
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
			//m_board->ResetStone();

			//std::cout<<"object size : " << m_objectList.size() << std::endl;

			BoardManager::GetInstance().SetStoneType(StoneType::Black);
			BoardManager::GetInstance().SetStoneAbility(StoneAbility::JokerAbility1);
			BoardManager::GetInstance().InputBasedGameLoop({ 70, 70 });

			Board board = BoardManager::GetInstance().GetBoard();
			int boardSize = BoardManager::GetInstance().GetBoardSize();

			for (int r = 0; r < boardSize; ++r)
			{
				for (int c = 0; c < boardSize; ++c)
				{
					if (board[r][c])
					{
						auto stone = board[r][c];
						m_objectList.emplace_back(stone.get());
					}
				}
			}

		};

	m_commandMap["F4"] = [this]()
		{
			//m_board->SpawnStone(5);


			//std::cout << "F4 Command Received" << std::endl;
		};

	m_commandMap["F5"] = [this]()
		{
			SceneManager::GetInstance().ChangePostProcessing("PassThrough");
		};

	m_commandMap["Go"] = [this]()
		{

			//if (m_player) m_player->MoveUp();
			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(0, -1, 0, 0));

		};

	m_commandMap["Back"] = [this]()
		{

			//if (m_player) m_player->MoveDown();
			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(0, 1, 0, 0));

		};

	m_commandMap["Left"] = [this]()
		{

			//if (m_player) m_player->MoveLeft();
			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(-1, 0, 0, 0));

		};

	m_commandMap["Right"] = [this]()
		{

			//if (m_player) m_player->MoveRight();
			m_moveDir = DirectX::XMVectorAdd(m_moveDir, DirectX::XMVectorSet(1, 0, 0, 0));

		};
}

void GameScene::OnInput(const MouseEvent& ev) // mouseInput
{
	if (ev.type == MouseType::LDown)
	{

		std::cout << ev.pos.x << " " << ev.pos.y << std::endl;

		//auto [row, col] = m_boardObj->ScreenToBoard(ev.pos.x, ev.pos.y);


		//if (!m_board->IsOnBoard(row, col))  return;
		//if (m_board->GetStone(row, col).color != StoneColor::None) return;


		//bool ok = m_board->PlaceStone(row, col, { StoneColor::Black, StoneAbility::None });
  //  
		//if (!ok) return;
	}
	//if (ev.type == MouseType::LDown)
	//{
	//	std::cout << ev.pos.x << " " << ev.pos.y << std::endl;

	//	auto [row, col] = m_boardObj->ScreenToBoard(ev.pos.x, ev.pos.y);


	//	if (!m_board->IsOnBoard(row, col))                return;
	//	if (m_board->GetStone(row, col).color != StoneColor::None) return;


	//	bool ok = m_board->PlaceStone(row, col, { StoneColor::Special, StoneAbility::ability1 });
	//	if (!ok) return;
	//}
	else if (ev.type == MouseType::RDown)
	{
		std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
		//auto [row, col] = m_boardObj->ScreenToBoard(ev.pos.x, ev.pos.y);

		//if (!m_board->IsOnBoard(row, col)) return;
		//if (m_board->GetStone(row, col).color != StoneColor::None) return;

		//bool ok = m_board->PlaceStone(row, col, { StoneColor::Special ,StoneAbility::ability1 });
		//if (!ok) return;

	}

// 	else if (ev.type == MouseType::Move)
// 	{
// 		std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
// 	}

	for (auto& button : m_buttonList)
	{
		button->CheckInput(ev);

	}

}

void GameScene::DebugBoardState()
{
//	//------------------------------------------------------------------
//// 1. 열 번호 헤더
////------------------------------------------------------------------
//	std::cout << "   ";                           // 좌측 여백
//	for (int col = 0; col < BOARD_SIZE; ++col)
//		std::cout << std::setw(3) << col;
//	std::cout << '\n';
//
//	//------------------------------------------------------------------
//	// 2. 행 단위로 격자 출력
//	//------------------------------------------------------------------
//	for (int row = 0; row < BOARD_SIZE; ++row)
//	{
//		std::cout << std::setw(2) << row << ' ';  // 행 번호 출력
//
//		for (int col = 0; col < BOARD_SIZE; ++col)
//		{
//			auto info = m_board->GetStone(row, col);
//
//			char ch = '.';
//			if (info.color == StoneColor::Black)  ch = 'B';
//			else if (info.color == StoneColor::White)  ch = 'W';
//
//			std::cout << ' ' << ch << ' ';
//		}
//		std::cout << '\n';
//	}
//	std::cout << std::flush;
//	std::cout << "F3 Command Received" << std::endl;
}
