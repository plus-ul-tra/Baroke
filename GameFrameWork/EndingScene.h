#pragma once
#include "Engine.h"
#include "Player.h"
#include "BoardObject.h"
#include "BackGround.h"


class EndingScene : public SceneBase {

private:
	vector<unique_ptr<Button>> m_endingButtonList;
public:
	EndingScene() { std::cout << "Ending Scene" << std::endl; }
	~EndingScene() = default;

	void Initialize() override;
	void Update(double deltaTime) override;

	void OnEnter() override;
	void OnLeave() override;

	void OnCommand(std::string& cmd) override;

	void KeyCommandMapping() override;

	void OnInput(const MouseEvent& ev) override;
};