#include "cpch.h"
#include "TitleScene.h"


// init �̳� asset �ε�� Scene �����̹Ƿ� base�� ������ ����
void TitleScene::Initialize()
{






	std::cout << "titleScene �ʱ�ȭ �Ϸ�" <<std::endl;
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
