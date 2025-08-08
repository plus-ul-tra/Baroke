#pragma once
#include "Engine.h"
#include "Player.h"
#include "BoardObject.h"
#include "BackGround.h"

class TitleScene : public SceneBase {

	double m_elsapsedTime = 0.0;
	vector<unique_ptr<Button>> m_titleButtonList; // ¹öÆ°µé
protected:
	
public:
	//TitleScene() = default;
	TitleScene() { std::cout << "Title Scene" << std::endl; };
	~TitleScene() = default;


	void Initialize() override;
	void Update(double deltaTime) override;
	//void FixedUpdate(double fixedDeltaTime) override;
	//void Render(Renderer& renderer) override;

	void OnEnter() override;

	void OnLeave() override;

	void OnCommand(std::string& cmd) override;


	void KeyCommandMapping() override;

	void OnInput(const MouseEvent& ev) override;
};