#include "cpch.h"
#include "TitleScene.h"


// init 이나 asset 로드는 Scene 공통이므로 base로 뺄수도 있음
void TitleScene::Initialize()
{

	std::cout << "titleScene Init" <<std::endl;
	KeyCommandMapping();
}

void TitleScene::Update(double deltaTime)
{


	m_elsapsedTime += deltaTime;
	//test용 씬전환

// 	if (m_elsapsedTime > 3.0) {
// 
// 
// 		//SceneManager::GetInstance().LoadScene(std::string("Game1"));
// 		SceneManager::GetInstance().ChangeScene(std::string("Game1"));
// 
// 	}

}


void TitleScene::Render(Renderer& renderer)
{
	renderer.RenderBegin();

	renderer.RenderEnd();
}


void TitleScene::OnEnter()
{

	std::cout << "Title Scene OnEnter" << std::endl;
	
}

void TitleScene::OnLeave()
{
	std::cout << "Title Scene Left" << std::endl;
	Reset();
}

void TitleScene::OnCommand(std::string& cmd)
{
	auto it = m_commandMap.find(cmd);
	if (it != m_commandMap.end())
	{
		it->second(); // 함수 실행
	}
	else
	{
		std::cout << "Unknown Command: " << cmd << std::endl;
	}
}

void TitleScene::KeyCommandMapping()
{
	m_commandMap["Escape"] = [this]()
		{
			std::cout << "Escape Command Received: Pausing Game" << std::endl;
			// 추후 일시정지/재개 로직 여기에
		};

	m_commandMap["F1"] = [this]()
		{
			SceneManager::GetInstance().ChangeScene(std::string("Game1"));
		};

	m_commandMap["F2"] = []()
		{
			std::cout << "F2 Command Received" << std::endl;
		};

	m_commandMap["F3"] = []()
		{
			std::cout << "F3 Command Received" << std::endl;
		};

	m_commandMap["F4"] = []()
		{
			std::cout << "F4 Command Received" << std::endl;
		};
}

