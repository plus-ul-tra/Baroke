#pragma once
#include "Engine.h"
#include "Player.h"
#include "Board.h"
#include "BoardObject.h"

class GameScene : public SceneBase { 
	
private:
	Player* m_player = nullptr;
	BoardObject* m_boardObj = nullptr;
	std::unique_ptr<Board> m_board;
	DirectX::XMVECTOR m_moveDir = DirectX::XMVectorZero();

	std::vector<std::unique_ptr<Button>> m_buttonList;

	int m_stageNo = 0;		// 스테이지 단계
	int m_BlackStone = 10;	// 착수 가능한 흑돌
	int m_whiteLeft = 0;	// 남은 흰돌 개수
	int m_money = 0;		// 보유중인 흰 돌 = 머니
	 
	void  StartStage();				// 스테이지 시작
	void  CheckStageClear();		// 스테이지 클리어 체크
	int   CountWhite() const;		// 스테이지 클리어 체크용 내부 함수
	int	  CountBlack() const;		// 착수한 흑돌 개수 체크
	//stoneType T = stoneType::Black;


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

	void DebugBoardState();
	void RegisterJokerFunctions();
};