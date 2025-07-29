#pragma once
#include "Engine.h"
#include "Player.h"
#include "Board.h"
#include "BoardObject.h"
class GameScene : public SceneBase { 
	
private:
	Player* m_player = nullptr;
	std::unique_ptr<Board> m_board;
	DirectX::XMVECTOR m_moveDir = DirectX::XMVectorZero();


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