#pragma once
#include "Engine.h"
#include "Player.h"
#include "BoardObject.h"
#include "BackGround.h"
#include "HintMark.h"

class TutoScene : public SceneBase {

private:
	Player* m_player = nullptr;  // �̰� �÷��̾� �÷��̾ �ƴ϶� �Ǵ���
	BoardObject* m_boardObj = nullptr;
	BoardManager& m_board = BoardManager::GetInstance(); // �̱��� ���� �Ŵ���

	DirectX::XMVECTOR m_moveDir = DirectX::XMVectorZero();

	UIMode        m_uiMode = UIMode::Normal;


	int m_stageNo = 0;		// �������� �ܰ�
	int m_whiteLeft = 0;	// ���� �� ����

	void initGame(); // ���� �ʱ�ȭ

	void  SetUIButton(); // UI ��Ŀ ��ư ����

	void  CheckSlot(); 
	void  ModeCheck();
	void CRTAccess();

	void InitShop(); // ���� �ʱ�ȭ
	vector<unique_ptr<JokerButton>> m_jokerButtons; // ��Ŀ ��ư�� // ���߿� �ٸ� �������?

	//vector <unique_ptr<Button>>   m_normalUI;  // scene ��ȯ�� texture �ٲ� �����

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



	vector<pair<StoneAbility, JokerStoneInfo>> m_shopStones; // �� ��Ŀ

	unique_ptr<ShopJokerButton> m_shopJokerButtons[24] = {};

	enum class GameState { Stage, ShopEnter, Shop, ShopExit, Ending };
	GameState m_gameState = GameState::Stage;
	float m_gameStateDelayElapsed = 0.0f; // ���� ���� ��ȯ ������ ��� �ð�
	float m_gameStateDelay = 3.0f; // ���� ���� ��ȯ ������

	int   m_lastIndex = -1;
	void ShopStage();

	std::vector<std::unique_ptr<HintMark>> m_hintPool;
	void SyncPlacementHintsToPool();  // ���̶���Ʈ ����ȭ
	void SetHintpool();

	Sound* m_shopBgm = nullptr; // ���� �������
	Channel* m_shopChannel = nullptr; // ���� ä��

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