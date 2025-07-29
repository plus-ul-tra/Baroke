#include "cpch.h"
#include "GameScene.h"


#define BOARD_SIZE 15

void GameScene::Initialize()
{
	std::cout << "Game Scene Init" << std::endl;
	KeyCommandMapping();
	m_board = CreateBoard(BOARD_SIZE);
	auto boardObj = std::make_unique<BoardObject>( 
		m_board.get(), 510, 90, 900, 900, 10);
	m_objectList.emplace_back(std::move(boardObj));


	m_board->PlaceStone(6, 5, { StoneColor::Black ,StoneAbility::None });
	m_board->PlaceStone(1, 1, { StoneColor::White ,StoneAbility::None });

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
			std::cout << "F2 Command Received" << std::endl;
		};

	m_commandMap["F3"] = [this]()
		{
			//------------------------------------------------------------------
			// 1. 열 번호 헤더
			//------------------------------------------------------------------
			std::cout << "   ";                           // 좌측 여백
			for (int col = 0; col < BOARD_SIZE; ++col)
				std::cout << std::setw(3) << col;
			std::cout << '\n';

			//------------------------------------------------------------------
			// 2. 행 단위로 격자 출력
			//------------------------------------------------------------------
			for (int row = 0; row < BOARD_SIZE; ++row)
			{
				std::cout << std::setw(2) << row << ' ';  // 행 번호 출력

				for (int col = 0; col < BOARD_SIZE; ++col)
				{
					auto info = m_board->GetStone(row, col); 

					char ch = '.';
					if (info.color == StoneColor::Black)  ch = 'B';
					else if (info.color == StoneColor::White)  ch = 'W';

					std::cout << ' ' << ch << ' ';
				}
				std::cout << '\n';
			}
			std::cout << std::flush;
			std::cout << "F3 Command Received" << std::endl;
		};

	m_commandMap["F4"] = [this]()
		{
			m_board->PlaceStone(0, 1, { StoneColor::Black ,StoneAbility::None });

			// 새로운 오브젝트 생성
			unique_ptr<NewObject> newObj = std::make_unique<NewObject>(100, 100, 50, 50, "Sample.png", 50);
			m_objectList.emplace_back(std::move(newObj));

			m_board->PlaceStone(1, 0, { StoneColor::Black ,StoneAbility::None });
			m_board->PlaceStone(1, 2, { StoneColor::Black ,StoneAbility::None });
			m_board->PlaceStone(2, 1, { StoneColor::Black ,StoneAbility::None });
			std::cout << "F4 Command Received" << std::endl;
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
	if (ev.type == MouseType::LUp)
	{
		std::cout << ev.pos.x << " " << ev.pos.y << std::endl;
	}
}