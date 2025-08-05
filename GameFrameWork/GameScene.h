#pragma once
#include "Engine.h"
#include "Player.h"
#include "BoardObject.h"
#include "BackGround.h"

class GameScene : public SceneBase { 
	
private:
	Player* m_player = nullptr;
	BoardObject* m_boardObj = nullptr;
	BoardManager& m_board = BoardManager::GetInstance(); // 싱글톤 보드 매니저

	DirectX::XMVECTOR m_moveDir = DirectX::XMVectorZero();

 	UIMode        m_uiMode = UIMode::Normal;


	int m_stageNo = 0;		// 스테이지 단계
	int m_BlackStone = 500;	// 착수 가능한 흑돌
	int m_whiteLeft = 0;	// 남은 흰돌 개수
	
	void SetUIJokerButton(); // UI 조커 버튼 설정
 	void  StartStage();				// 스테이지 시작
 	void  CheckStageClear();		// 스테이지 클리어 체크

	void  ModeCheck();

	void ShopStage(); // 사점


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

// 	void EnterSacrificeMode(StoneAbility ab)
// 	{
// 		m_uiMode = UIMode::Sacrifice;
// 		m_pendingAb = ab;
// 		//BoardObject::GetInstance().HighlightSelectableStones();
// 	}
// 	void ExitSacrificeMode() { m_uiMode = UIMode::Normal; }
};