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
	if (m_elsapsedTime > 5.0) {

		SceneManager::GetInstance().LoadScene(std::string("Game1"));
		SceneManager::GetInstance().ChangeScene(std::string("Game1"));

	}

}


void TitleScene::Render(Renderer& renderer)
{
	renderer.RenderBegin();

	renderer.RenderEnd();
}
