#include "cpch.h"
#include "GameScene.h"

void GameScene::Initialize()
{
	std::cout << "Game Scene Init" << std::endl;
	KeyCommandMapping();
	// 초기화를 매번 이렇게 할 순 없긴해
// 	auto playerObject = std::make_unique<Player>(
// 		600.0f, 300.0f, 50.0f, 50.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) // 녹색
// 	);
// 	m_objectList.push_back(std::move(playerObject));

	//실행 중 동적 생성/삭제는 어떻게?
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
			std::cout << "F3 Command Received" << std::endl;
		};

	m_commandMap["F4"] = [this]()
		{
			std::cout << "F4 Command Received" << std::endl;
		};

	m_commandMap["Go"] = [this]()
		{
			if (m_player) m_player->MoveUp();
		};

	m_commandMap["Back"] = [this]()
		{
			if (m_player) m_player->MoveDown();
		};

	m_commandMap["Left"] = [this]()
		{
			if (m_player) m_player->MoveLeft();
		};

	m_commandMap["Right"] = [this]()
		{
			if (m_player) m_player->MoveRight();
		};
}

