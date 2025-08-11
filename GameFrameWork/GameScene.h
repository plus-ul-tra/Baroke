#pragma once
#include "Engine.h"
#include "Player.h"
#include "BoardObject.h"
#include "BackGround.h"
#include "HintMark.h"

class GameScene : public SceneBase { 
	
private:
	Player* m_player = nullptr;  // 이거 플레이어 플레이어가 아니라 판다임
	BoardObject* m_boardObj = nullptr;
	BoardManager& m_board = BoardManager::GetInstance(); // 싱글톤 보드 매니저

	DirectX::XMVECTOR m_moveDir = DirectX::XMVectorZero();

 	UIMode        m_uiMode = UIMode::Normal;


	int m_stageNo = 0;		// 스테이지 단계
	int m_whiteLeft = 0;	// 남은 흰돌 개수
	
	void  SetUIButton(); // UI 조커 버튼 설정
 	void  StartStage();				// 스테이지 시작
 	void  CheckStageClear();		// 스테이지 클리어 체크
	void  CheckSlot();
	void  ModeCheck();

	void InitShop(); // 상점 초기화
	vector<unique_ptr<JokerButton>> m_jokerButtons; // 조커 버튼들 // 나중에 다른 방식으로?
	//vector <unique_ptr<Button>>   m_normalUI;  // scene 전환시 texture 바꿀 얘들임
	
	vector <unique_ptr<Button>>	  m_jokerSlot;
	vector <unique_ptr<Button>>	  m_itemSlot;
	vector <unique_ptr<Button>>	  m_passiveSlot;
	vector <unique_ptr<Object>>   m_useless;
	unique_ptr<Button>            m_desc;
	unique_ptr<Button>			  m_rightUI;
	unique_ptr<Button>			  m_leftUI;
	unique_ptr<Button>			  m_leftUpUI;
	unique_ptr<Button>			  m_cyber;

	vector<pair<StoneAbility, JokerStoneInfo>> m_shopStones; // 돌 조커
	vector<pair<StoneAbility, JokerStoneInfo>> m_shopItems; // 아이템 조커
	unique_ptr<ShopJokerButton> m_shopJokerButtons[6] = {};
	unique_ptr<ShopBuyStoneButton> m_shopBuyStoneButton;
	unique_ptr<ShopRerollButton> m_shopShopRerollButton;
	unique_ptr<ShopEndButton> m_shopExitButton;
	int m_shopRng[3] = { 80, 45, 15 }; // 상점 아이템 확률
	void ShopStage(); // 상점
	enum class GameState { Stage, ShopEnter, Shop, ShopExit };
	GameState m_gameState = GameState::Stage;
	float m_gameStateDelayElapsed = 0.0f; // 게임 상태 전환 딜레이 경과 시간
	float m_gameStateDelay = 3.0f; // 게임 상태 전환 딜레이

	int   m_lastIndex = -1;


	std::vector<std::unique_ptr<HintMark>> m_hintPool;
	void SyncPlacementHintsToPool();  // 하이라이트 동기화
	void SetHintpool();

	Sound* m_shopBgm = nullptr; // 상점 배경음악
	Channel* m_shopChannel = nullptr; // 상점 채널

public:
	//GameScene() = default;
	GameScene() { std::cout << "Game Scene" << std::endl; }
	~GameScene() = default;
	void Initialize() override;

	void FixedUpdate(double fixedDeltaTime) override;
	void Update(double deltaTime) override;
	void LateUpdate(double deltaTime) override;
	//void Render(Renderer& renderer) override;

	void OnEnter() override;

	void OnLeave() override;

	void OnCommand(std::string& cmd) override;

	void KeyCommandMapping() override;

	void OnInput(const MouseEvent& ev) override;
	void ChangeThema(int thema = -1);
	void ChangeOriginSlot(const std::string& bitmapFile);
};