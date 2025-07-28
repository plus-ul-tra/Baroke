#pragma once
#include "Engine.h"
#include "Player.h"

class TitleScene : public SceneBase {

private:
	double m_elsapsedTime = 0.0;
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


};