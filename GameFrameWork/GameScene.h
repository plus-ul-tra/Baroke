#pragma once
#include "Engine.h"
#include "Player.h"
class GameScene : public SceneBase { 
	
private:
	
public:
	//GameScene() = default;
	GameScene() { std::cout << "Game Scene" << std::endl; }
	~GameScene() = default;
	void Initialize() override;

	void FixedUpdate(double fixedDeltaTime);
	void Update(double deltaTime) override;
	void LateUpdate(double deltaTime) override;
	//void Render(Renderer& renderer) override;


	void OnCommand(std::string& cmd) override;

};