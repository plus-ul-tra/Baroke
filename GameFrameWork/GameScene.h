#pragma once
#include "Engine.h"
#include "Player.h"
#include "BoardObject.h"
#include "BackGround.h"

class GameScene : public SceneBase { 
	
private:
	Player* m_player = nullptr;
	BoardObject* m_boardObj = nullptr;
	BoardManager& m_board = BoardManager::GetInstance(); // �̱��� ���� �Ŵ���

	DirectX::XMVECTOR m_moveDir = DirectX::XMVectorZero();

	std::vector<std::unique_ptr<Button>> m_buttonList;

	int m_stageNo = 0;		// �������� �ܰ�
	int m_BlackStone = 500;	// ���� ������ �浹
	int m_whiteLeft = 0;	// ���� �� ����
	int m_money = 0;		// �������� �� �� = �Ӵ�
	
	void SetUIJokerButton(); // UI ��Ŀ ��ư ����
 	void  StartStage();				// �������� ����
 	void  CheckStageClear();		// �������� Ŭ���� üũ

public:
	//GameScene() = default;
	GameScene() { std::cout << "Game Scene" << std::endl; }
	~GameScene() = default;
	void Initialize() override;

	void FixedUpdate(double fixedDeltaTime);
	void Update(double deltaTime) override;
	void LateUpdate(double deltaTime) override;
	//void Render(Renderer& renderer) override;

	void OnEnter() override;

	void OnLeave() override;

	void OnCommand(std::string& cmd) override;

	void KeyCommandMapping() override;

	void OnInput(const MouseEvent& ev) override;

};