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
	int m_whiteLeft = 0;	// 남은 흰돌 개수
	
	void SetUIButton(); // UI 조커 버튼 설정
 	void  StartStage();				// 스테이지 시작
 	void  CheckStageClear();		// 스테이지 클리어 체크

	void  ModeCheck();

	void InitShop(); // 상점 초기화
	vector<unique_ptr<JokerButton>> m_jokerButtons; // 조커 버튼들 // 나중에 다른 방식으로?
	//vector <unique_ptr<Button>>   m_normalUI;  // scene 전환시 texture 바꿀 얘들임
	
	vector <unique_ptr<Button>>	  m_jokerSlot;
	vector <unique_ptr<Button>>	  m_itemSlot;
	vector <unique_ptr<Button>>	  m_passiveSlot;
	unique_ptr<Button>            m_desc;
	unique_ptr<Button>			  m_rightUI;
	unique_ptr<Button>			  m_leftUI;
	unique_ptr<Button>			  m_leftUpUI;


	vector<pair<StoneAbility, JokerStoneInfo>> m_shopStones; // 돌 조커
	vector<pair<StoneAbility, JokerStoneInfo>> m_shopItems; // 아이템 조커
	unique_ptr<ShopJokerButton> m_shopJokerButtons[6] = {};
	unique_ptr<ShopEndButton> m_shopExitButton;
	int m_shopRng[3] = { 80, 45, 15 }; // 상점 아이템 확률
	void ShopStage(); // 상점
	enum class GameState { Stage, ShopEnter, Shop };
	GameState m_gameState = GameState::Stage;

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
	void ChangeThema();
};