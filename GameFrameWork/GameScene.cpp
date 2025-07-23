#include "cpch.h"
#include "GameScene.h"

void GameScene::Initialize()
{
	//std::cout << "Game Scene 초기화 완료" << std::endl;
	// 초기화를 매번 이렇게 할 순 없긴해
	auto playerObject = std::make_unique<Player>(
		600.0f, 300.0f, 50.0f, 50.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) // 녹색
	);
	m_objectList.push_back(std::move(playerObject));

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

void GameScene::OnCommand(std::string& cmd)
{

}

