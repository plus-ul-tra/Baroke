#pragma once
#include "Engine.h"
#include "Player.h"
#include "BoardObject.h"
#include "BackGround.h"
#include "HintMark.h"

class TutoScene : public SceneBase {

private:
	Player* m_player = nullptr;  // 이거 플레이어 플레이어가 아니라 판다임
	BoardObject* m_boardObj = nullptr;
	BoardManager& m_board = BoardManager::GetInstance(); // 싱글톤 보드 매니저

	DirectX::XMVECTOR m_moveDir = DirectX::XMVectorZero();

	UIMode        m_uiMode = UIMode::Normal;


	int m_stageNo = 0;		// 스테이지 단계
	int m_whiteLeft = 0;	// 남은 흰돌 개수

	void initGame(); // 게임 초기화

	void  SetUIButton(); // UI 조커 버튼 설정

	void  CheckSlot(); 
	void  ModeCheck();
	void CRTAccess();

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
	unique_ptr<Button>			  m_BlackUI;
	unique_ptr<Button>			  m_WhiteUI;
	unique_ptr<Button>			  m_WaxUI;
	unique_ptr<Button>			  m_leftUpUI;
	unique_ptr<Button>			  m_cyber;



	vector<pair<StoneAbility, JokerStoneInfo>> m_shopStones; // 돌 조커

	unique_ptr<ShopJokerButton> m_shopJokerButtons[24] = {};

	enum class GameState { Stage, ShopEnter, Shop, ShopExit, Ending };
	GameState m_gameState = GameState::Stage;
	float m_gameStateDelayElapsed = 0.0f; // 게임 상태 전환 딜레이 경과 시간
	float m_gameStateDelay = 3.0f; // 게임 상태 전환 딜레이

	int   m_lastIndex = -1;
	void ShopStage();

	std::vector<std::unique_ptr<HintMark>> m_hintPool;
	void SyncPlacementHintsToPool();  // 하이라이트 동기화
	void SetHintpool();

public:
	//GameScene() = default;
	TutoScene() { std::cout << "Game Scene" << std::endl; }
	~TutoScene() = default;
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
};
