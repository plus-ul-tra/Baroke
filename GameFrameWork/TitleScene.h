#pragma once
#include "Engine.h"
#include "Player.h"
#include "BoardObject.h"
#include "BackGround.h"


class TitleScene : public SceneBase {

	double m_elsapsedTime = 0.0;
	vector<unique_ptr<Button>> m_titleButtonList; // 버튼들

	Button* m_creditBtn;
	Button* m_tutorialImageBtn;
	Button* m_nextButton; // 다음 버튼
	Button* m_prevButton; // 이전 버튼
	int m_tutorialIndex = 0; // 튜토리얼 이미지 인덱스

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


	void TutorialTextureChange()
	{
		if (m_tutorialIndex > 18) { m_tutorialIndex %= 19; }
		if (m_tutorialIndex == -1) { m_tutorialIndex = 18; }
		m_tutorialImageBtn->GetComponent<BitmapRender3D>()->ChangeTexture("Tutorial_" + std::to_string(m_tutorialIndex) + ".png");
	}
};