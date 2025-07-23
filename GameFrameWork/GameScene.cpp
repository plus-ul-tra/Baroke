#include "cpch.h"
#include "GameScene.h"

void GameScene::Initialize()
{
	//std::cout << "Game Scene �ʱ�ȭ �Ϸ�" << std::endl;
	// �ʱ�ȭ�� �Ź� �̷��� �� �� ������
	auto playerObject = std::make_unique<Player>(
		600.0f, 300.0f, 50.0f, 50.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) // ���
	);
	m_objectList.push_back(std::move(playerObject));

	//���� �� ���� ����/������ ���?
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

