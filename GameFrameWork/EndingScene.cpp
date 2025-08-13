#include "cpch.h"
#include "EndingScene.h"
using namespace DirectX;
void EndingScene::Initialize()
{
	// 사용할 것들
	KeyCommandMapping();

	SceneManager::GetInstance().ChangePostProcessing("CRTOn");
	m_filterElsapsedTime = 0.0f;
	m_isFilterQueue = true;
	m_isExitrQueue = false;

	m_channel->stop();
	m_bgm = nullptr;
	m_channel = nullptr;
	m_bgm = SoundManager::GetInstance().GetSound("lose.wav"); //사운드 교체
	m_bgm->setMode(FMOD_LOOP_OFF);
	m_soundManager.GetSystem()->getChannel(1, &m_channel);
	m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);
}

void EndingScene::Update(double deltaTime)
{
	m_filterElsapsedTime += deltaTime;

	for (auto& notUniqueObject : m_notUniqueObjectList) notUniqueObject->Update(deltaTime);

	if (m_isFilterQueue && m_filterElsapsedTime > 0.8f)
	{
		m_isFilterQueue = false;
		SceneManager::GetInstance().ChangePostProcessing("CRTNoise");
	}

	if (SceneManager::GetInstance().IsExit())
	{
		if (!m_isExitrQueue)
		{
			m_isExitrQueue = true;
			m_filterElsapsedTime = 0.0f;
			SceneManager::GetInstance().ChangePostProcessing("CRTOff");
		}
		if (m_isExitrQueue && m_filterElsapsedTime > 1.3f)
		{
			m_isExitrQueue = false;
			SceneManager::GetInstance().ChangeSceneToNext();
		}
	}
}

void EndingScene::OnEnter()
{
	Initialize();
	
	unique_ptr<BackGround> backGround = std::make_unique<BackGround>(0, 0, 1920, 1080);
	m_objectList.emplace_back(std::move(backGround));
	XMFLOAT4 color = { 0.3f, 0.3f, 0.3f, 1.0f };
	Mediator::GetInstance().SetBackGroundColor(color, color);

	unique_ptr<Button> end = std::make_unique<Button>(0.0f, 200.0f, 521.0f, 119.0f, "T_GameOver_Main.png");
	m_notUniqueObjectList.emplace_back(end.get());
	m_endingButtonList.emplace_back(std::move(end));

	unique_ptr<Button> jokerCount = std::make_unique<Button>(0.0f, 0.0f, 188.0f, 35.0f, "T_GameOver_Used_Joker_Stone.png");
	m_notUniqueObjectList.emplace_back(jokerCount.get());
	m_endingButtonList.emplace_back(std::move(jokerCount));

	//--Text Here

	unique_ptr<Button> whiteGet = std::make_unique<Button>(0.0f, -100.0f, 96.0f, 35.0f, "T_GameOver_Captured_Stone.png");
	m_notUniqueObjectList.emplace_back(whiteGet.get());
	m_endingButtonList.emplace_back(std::move(whiteGet));

	//--Text Here

	unique_ptr<Button> score = std::make_unique<Button>(0.0f, -200.0f, 119.0f, 35.0f, "T_GameOver_Final_Score.png");
	m_notUniqueObjectList.emplace_back(score.get());
	m_endingButtonList.emplace_back(std::move(score));

	//--Text Here

	unique_ptr<Button> returnButton = std::make_unique<SceneChangeButton>(0.0f, -350.0f, 212.0f, 42.0f, "T_GameOver_Main_Screen.png", "Title",buttonType::EndingToTitle);
	m_notUniqueObjectList.emplace_back(returnButton.get());
	m_endingButtonList.emplace_back(std::move(returnButton));
}

void EndingScene::OnLeave()
{
	Reset();

	m_notUniqueObjectList.clear();

	m_channel->stop();
	m_bgm = nullptr;
	m_channel = nullptr;
}

void EndingScene::OnCommand(std::string& cmd)
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

void EndingScene::KeyCommandMapping()
{
	m_commandMap["F1"] = [this]()
		{

		};

	m_commandMap["F2"] = []()
		{
			SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
		};

	m_commandMap["F3"] = []()
		{
			std::cout << "F3 Command Received" << std::endl;
		};

	m_commandMap["F4"] = []()
		{
			std::cout << "F4 Command Received" << std::endl;
		};
	m_commandMap["F5"] = []()
		{
			SceneManager::GetInstance().ChangePostProcessing("PassThrough");
		};
}

void EndingScene::OnInput(const MouseEvent& ev)
{
	for (auto& button : m_endingButtonList)
	{
		button->CheckInput(ev);
	}
}

