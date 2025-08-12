#include "cpch.h"
#include "TitleScene.h"


// init 이나 asset 로드는 Scene 공통이므로 base로 뺄수도 있음
void TitleScene::Initialize()
{
	std::cout << "titleScene Init" <<std::endl;
	KeyCommandMapping();

	unique_ptr<BackGround> backGround = std::make_unique<BackGround>(0, 0, 1920, 1080);
	m_objectList.emplace_back(std::move(backGround));

	XMFLOAT4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
	Mediator::GetInstance().SetBackGroundColor(color, color);
	SceneManager::GetInstance().ChangePostProcessing("CRTFilter");

	// 팝업




	//title
	unique_ptr<Button> title = std::make_unique<Button>(9.0f, 0.0f, 798, 385, "T_Main_Logo.png");
	m_notUniqueObjectList.emplace_back(title.get());
	m_titleButtonList.emplace_back(std::move(title));

	unique_ptr<Button> creditImage = std::make_unique<Button>(0.f, -0.f, 1358.f, 742.f, "T_Main_Credit.png");
	creditImage->GetComponent<BitmapRender3D>()->SetActive(true); // 시작은 숨김
	creditBtn = creditImage.get();
	m_buttonList.emplace_back(creditImage.get());
	m_notUniqueObjectList.emplace_back(creditImage.get());
	m_titleButtonList.emplace_back(std::move(creditImage));

	unique_ptr<CreditButton> creditButton = std::make_unique<CreditButton>(-780.0f, -480.0f, 315.f, 144.f, "T_Credit.png");
	creditButton->resisterOtherButton(creditBtn);
	m_buttonList.emplace_back(creditButton.get());
	m_notUniqueObjectList.emplace_back(creditButton.get());
	m_titleButtonList.emplace_back(std::move(creditButton));

	unique_ptr<SceneChangeButton> tutoButton = std::make_unique<SceneChangeButton>(-390, -480.0f, 315.f, 144.f, "T_Main_Tutorial.png", "Game1", buttonType::TitleToTutorial); // 일단 임시로 게임1
	m_buttonList.emplace_back(tutoButton.get());
	m_notUniqueObjectList.emplace_back(tutoButton.get());
	m_titleButtonList.emplace_back(std::move(tutoButton));

	unique_ptr<SceneChangeButton> startButton = std::make_unique<SceneChangeButton>(-0, -480.0f, 315.f, 144.f, "T_Main_Start_Select.png", "Game1", buttonType::TitleToGame);
	m_buttonList.emplace_back(startButton.get());
	m_notUniqueObjectList.emplace_back(startButton.get());
	m_titleButtonList.emplace_back(std::move(startButton));

	unique_ptr<SettingButton> settingButton = std::make_unique<SettingButton>(390, -480.0f, 315.f, 144.f, "T_Main_Setting.png");
	m_buttonList.emplace_back(settingButton.get());
	m_notUniqueObjectList.emplace_back(settingButton.get());
	m_titleButtonList.emplace_back(std::move(settingButton));

	unique_ptr<ExitButton> exitButton = std::make_unique<ExitButton>(780.0f, -480.0f, 315.f, 144.f, "T_Main_Exit.png");
	m_buttonList.emplace_back(exitButton.get());
	m_notUniqueObjectList.emplace_back(exitButton.get());
	m_titleButtonList.emplace_back(std::move(exitButton));



	// 사운드
	m_channel->stop();
	m_bgm = nullptr;
	m_channel = nullptr;
	m_bgm = SoundManager::GetInstance().GetSound("MainBGM.mp3");
	m_bgm->setMode(FMOD_LOOP_NORMAL);
	m_soundManager.GetSystem()->getChannel(0, &m_channel);
	m_soundManager.GetSystem()->playSound(m_bgm, nullptr, false, &m_channel);
}

void TitleScene::Update(double deltaTime)
{
	m_elsapsedTime += deltaTime;
	for (auto& notUniqueObject : m_notUniqueObjectList)
	{
		notUniqueObject->Update(deltaTime);
	}
	//test용 씬전환
}


void TitleScene::OnEnter()
{
	XMFLOAT4 color = { 0.f, 1.f, 1.f, 1.0f };
	Mediator::GetInstance().SetBackGroundColor(color, color);

	SceneManager::GetInstance().ChangePostProcessing("CRTFilter");
	std::cout << "Title Scene OnEnter" << std::endl;
	Initialize(); // 필요시 수정
}

void TitleScene::OnLeave()
{
	std::cout << "Title Scene Left" << std::endl;
	Reset();

	m_notUniqueObjectList.clear();

	// 사운드 초기화
	m_channel->stop();
	m_bgm = nullptr;
	m_channel = nullptr;
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


			SceneManager::GetInstance().ChangeScene(std::string("Ending"));
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

void TitleScene::OnInput(const MouseEvent& ev)
{
	for (auto& button : m_buttonList)
	{
		button->CheckInput(ev);
	}
}

