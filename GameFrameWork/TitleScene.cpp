#include "cpch.h"
#include "TitleScene.h"


// init 이나 asset 로드는 Scene 공통이므로 base로 뺄수도 있음
void TitleScene::Initialize()
{
	std::cout << "titleScene 초기화 완료" <<std::endl;
}

void TitleScene::Update(double deltaTime)
{

	m_elsapsedTime += deltaTime;
	if (m_elsapsedTime > 100.0) {

		SceneManager::GetInstance().LoadScene(std::string("Game1"));
		SceneManager::GetInstance().ChangeScene(std::string("Game1"));

	}

}


void TitleScene::Render(Renderer& renderer)
{
	renderer.RenderBegin();

	renderer.RenderEnd();
}

void TitleScene::OnCommand(std::string& cmd)
{
	if (cmd == "Escape")
	{
		std::cout << "Escape Command Received: Pausing Game" << std::endl;
// 		if (m_isPaused)
// 		{
// 			Resume();
// 			std::cout << "Game Resumed" << std::endl;
// 		}
// 		else
// 		{
// 			Pasued();
// 			std::cout << "Game Paused" << std::endl;
// 		}
	}
	else if (cmd == "F1")
	{

		std::cout << "F1 Command Received" << std::endl;



	}
	else if (cmd == "F2")
	{
		std::cout << "F2 Command Received" << std::endl;
	}
	else if (cmd == "F3")
	{
		std::cout << "F3 Command Received" << std::endl;
// 		if (auto obj = FindObject("Player"))
// 		{
// 			if (auto fsm = obj->GetComponent<FSMComponent>())
// 			{
// 				fsm->Trigger("RunStop");
// 			}
// 			if (auto tf = obj->GetComponent<TransformComponent>())
// 			{
// 				tf->SetPosition(200.f, 300.f);
// 				auto pos = tf->GetPosition();
// 				std::cout << "Player Pos: " << pos.x << ", " << pos.y << "\n";
// 			}
// 		}
	}
	else if (cmd == "F4")
	{
		std::cout << "F4 Command Received" << std::endl;
	}
	else
	{
		std::cout << "Unknown Command: " << cmd << std::endl;
	}
}
