#include "pch.h"
#include "Engine.h"
#include "Button.h"
#include "Mediator.h"


BitmapRender3D* Button::GetTextObjectRender() const
{
	if (m_bitmapRender->IsActive() && m_isHovered && m_textObject) return m_textObject->GetComponent<BitmapRender3D>();

	return nullptr;
}
void Button::RegistClickedTexture(const std::string& bitmapFile,int index)
{
	m_selectedTexture = bitmapFile;
	m_hasSelected = true;
	m_index = index;
}

void Button::CheckInput(const MouseEvent& mouseEvent)
{
	POINT mousePos = mouseEvent.pos;
	bool isInX =
		(
			mousePos.x >= m_transform->GetPosition().m128_f32[0] - m_width / 2
			&& mousePos.x <= m_transform->GetPosition().m128_f32[0] + m_width / 2
		);

	bool isInY =
		(
			mousePos.y >= m_transform->GetPosition().m128_f32[1] - m_height / 2
			&& mousePos.y <= m_transform->GetPosition().m128_f32[1] + m_height / 2
		);

	if (isInX && isInY)
	{
		m_isHovered = true;

		m_inputType = mouseEvent.type;
		if (m_inputType == MouseType::RDown) m_isPressedRight = true;
		else m_isPressedRight = false;

		if (!m_isActive) { return; }

		if (m_inputType == MouseType::LDown) { m_isPressed = true;}
		else m_isPressed = false;
	}
	else
	{
		m_isHovered = false;
	}
}

void Button::Selected()
{
	m_bitmapRender->ChangeBoardTexture(m_selectedTexture);
}
void Button::UnSelected() {
	m_bitmapRender->ChangeTexture(m_originTexture);
}

void Button::ButtonFunction(){

	if (!m_hasSelected) {
		return;
	}

	if (m_isPressed) {
		//m_isSelected = true;
		Mediator::GetInstance().SetSlotIndex(m_index);
		m_isPressed = false;
		
	}

}

void Button::Render(Renderer& renderer)
{
	if (m_bitmapRender->IsActive())
	{
		if (m_shopIcon) m_shopIcon->GetComponent<BitmapRender3D>()->Render(renderer);
		if (m_priceIcon) m_priceIcon->GetComponent<BitmapRender3D>()->Render(renderer);
	}
}

void JokerButton::ButtonFunction()
{
	if (m_isPressed && m_isActive && m_jokerAbility != StoneAbility::None)
    {
		std::cout << "button click" << std::endl;

		BoardManager::GetInstance().SetMode(UIMode::Sacrifice);
		BoardManager::GetInstance().SetStoneType(m_stoneType);
		BoardManager::GetInstance().SetStoneAbility(m_jokerAbility);
/*		BoardManager::GetInstance().ComputePlacementHints(m_jokerAbility);*/
		m_isPressed = false;
    }

	if (m_isPressedRight)
	{
		m_stoneType = StoneType::Joker; // 돌 타입 초기화
		m_jokerAbility = StoneAbility::None; // 능력 초기화
		m_bitmapRender->ChangeTexture(m_jokerInfoMap[StoneAbility::None].fileName); // 기본 이미지로 변경
		BindEnabledPredicate(BuildPredicate(StoneAbility::None));
		m_textObject = nullptr;
		m_isPressedRight = false;
	}
}

void JokerButton::SetButtonJoker(StoneType stoneType, StoneAbility ability)
{
	m_jokerAbility = ability;
	m_stoneType = stoneType;

	m_bitmapRender->ChangeTexture(m_jokerInfoMap[ability].fileName);
	m_textObject = nullptr;
	m_textObject = make_unique<NewObject>(m_transform->GetPosition().m128_f32[0] - 345.5f, m_transform->GetPosition().m128_f32[1] - 100.0f, 691.0f, 200.0f, 0.0f, m_jokerInfoMap[ability].toolTipName);

	BindEnabledPredicate(BuildPredicate(ability));
}

void ShopJokerButton::SetButtonJoker(JokerStoneInfo jokerInfo, StoneAbility ability)
{
	m_jokerInfo = jokerInfo;
	m_jokerAbility = ability;

	m_bitmapRender->ChangeTexture(jokerInfo.fileName);
	m_textObject = nullptr;

	int offset = 1;
	if (flag) offset = -1;
	
	
	m_textObject = make_unique<NewObject>(m_transform->GetPosition().m128_f32[0] - 300.0f* offset, m_transform->GetPosition().m128_f32[1] - 100.0f, 691.0f, 200.0f, 0.0f, m_jokerInfoMap[ability].toolTipName);
}

void ShopBuyStoneButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		if (m_boardManager.m_playerInfo.m_waxMoney >= 1 + m_boardManager.m_playerInfo.m_blackStoneUpgrade * 2)
		{
			m_boardManager.m_playerInfo.m_BlackStone += 2;
			m_boardManager.m_playerInfo.m_waxMoney -= 1 + m_boardManager.m_playerInfo.m_blackStoneUpgrade * 2;
			m_boardManager.m_playerInfo.m_blackStoneUpgrade++;

			m_isPressed = false;
		}
	}
}

void ShopBuyStoneButton::SetShowAndActive(bool active)
{
	if (m_bitmapRender) m_bitmapRender->SetActive(active);

	m_isActive = active;
}

void ShopRerollButton::ButtonFunction()
{
	if (m_isPressed && m_isActive && !m_isRerollButtonPressed)
	{
		if (m_boardManager.m_playerInfo.m_money >= m_boardManager.m_playerInfo.m_rerollCount)
		{
			m_boardManager.m_playerInfo.m_money -= m_boardManager.m_playerInfo.m_rerollCount;

			if (m_boardManager.m_playerInfo.m_rerollCount < 5) m_boardManager.m_playerInfo.m_rerollCount++;

			m_isPressed = false;
			m_isRerollButtonPressed = true;
		}
	}
}

void ShopRerollButton::SetShowAndActive(bool active)
{
	if (m_bitmapRender) m_bitmapRender->SetActive(active);

	m_isActive = active;
}

bool ShopRerollButton::IsReroll()
{
	if (!m_isRerollButtonPressed) return false;
	m_isRerollButtonPressed = false;
	return true;
}

void ShopJokerButton::SetShowAndActive(bool active)
{
	if (m_bitmapRender) m_bitmapRender->SetActive(active);

	m_isActive = active;
}

void ShopJokerButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		if (m_boardManager.m_playerInfo.m_money >= m_jokerInfo.costWhite)
		{
			bool isJokerFull = true; // 조커 버튼이 꽉 찼는지 여부
			if (m_jokerInfo.isStone)
			{
				for (int i = 0; i < 5; ++i)
				{
					auto jokerButton = (*m_jokerButton)[i].get();
					if (!jokerButton) continue;
					if (jokerButton->GetJokerAbility() != StoneAbility::None) continue; // 이미 능력이 있는 조커 버튼은 건너뜀

					jokerButton->SetButtonJoker(m_jokerInfo.stoneType, m_jokerAbility);
					m_boardManager.m_playerInfo.m_money -= m_jokerInfo.costWhite;

					isJokerFull = false; // 조커 버튼이 꽉 차지 않았음

					break;
				}
				SetShowAndActive(isJokerFull); // 상점 조커 버튼 숨김
			}
			else
			{
				for (int i = 5; i < 8; ++i)
				{
					auto jokerButton = (*m_jokerButton)[i].get();
					if (!jokerButton) continue;
					if (jokerButton->GetJokerAbility() != StoneAbility::None) continue;

					jokerButton->SetButtonJoker(m_jokerInfo.stoneType, m_jokerAbility);
					m_boardManager.m_playerInfo.m_money -= m_jokerInfo.costWhite;
					isJokerFull = false;
					break;
				}
				SetShowAndActive(isJokerFull);
			}

			std::cout << "Money : " << m_boardManager.m_playerInfo.m_money << std::endl;
		}
		else
		{
			std::cout << "Not enough money to buy this joker." << std::endl;
		}

		m_isPressed = false;
	}

	if (m_isHovered && m_isActive)
	{

	}
}

bool ShopEndButton::IsEndButtonPressed()
{
	if (!m_isEndButtonPressed) return false;

	m_isEndButtonPressed = false;
	return true;
}

void ShopEndButton::SetShowAndActive(bool active)
{
	if (m_bitmapRender) m_bitmapRender->SetActive(active);

	m_isActive = active;
	m_isEndButtonPressed = false;
}

void ShopEndButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		m_isEndButtonPressed = true;
		std::cout << "Shop End Button Pressed" << std::endl;

		m_isPressed = false;
	}
}

void SceneChangeButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		SceneManager::GetInstance().SetExit(true, m_sceneName); // 씬 전환을 위해 종료 플래그 설정
		m_isPressed = false;
	}

	switch (m_buttonType) {
	case buttonType::TitleToGame: // 타이틀에서 게임으로 전환
	{
		if (m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("T_Main_Start_Select.png");

		else if (!m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("T_Main_Start.png");
	}
	break;
	case buttonType::TitleToTutorial: // 타이틀에서 튜토리얼로 전환
	{
		if (m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("T_Main_Tutorial_Select.png");
		else if (!m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("T_Main_Tutorial.png");
	}
	break;
	case buttonType::TutorialToTitle: // 타이틀에서 튜토리얼로 전환
	{
		if (m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("TUTORIALEXIT_sel.png");
		else if (!m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("TUTORIALEXIT.png");
	}
	break;
	case buttonType::GameToEnding: // 타이틀에서 튜토리얼로 전환
	{
		if (m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("T_Common_Left_Down_Exit_Select.png");
		else if (!m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("T_Common_Left_Down_Exit.png");
	}
	break;
	case buttonType::EndingToTitle: // 타이틀에서 튜토리얼로 전환
	{
		if (m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("T_GameOver_Main_Screen_Select.png");
		else if (!m_isHovered && m_isActive) m_bitmapRender->ChangeTexture("T_GameOver_Main_Screen.png");
	}
	break;
	default:
		break;
	}
}

void GameEndButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		SceneManager::GetInstance().ChangeScene(std::string("Ending"));
		m_isPressed = false;
	}
}

void CreditButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		m_isToggle = !m_isToggle; // 토글 상태 변경
		m_otherButton->GetComponent<BitmapRender3D>()->SetActive(m_isToggle);
		m_isPressed = false;
	}

	if (m_isHovered && m_isActive)
	{
		m_bitmapRender->ChangeTexture("T_Main_Credit_Select.png");
	}
	else if (!m_isHovered && m_isActive)
	{
		m_bitmapRender->ChangeTexture("T_Credit.png");
	}
}



void RoleButton::ButtonFunction()
{

	if (m_isPressed && m_isActive)
	{
		std::cout << "Setting Button Pressed" << std::endl;
		m_isToggle = !m_isToggle; // 토글 상태 변경
		for (auto& btns : m_linkedButtons) 
		{
			if (btns) btns->GetComponent<BitmapRender3D>()->SetActive(m_isToggle);
		}
		m_isPressed = false;
	}

	if (m_isHovered && m_isActive)
	{
		m_bitmapRender->ChangeTexture("T_Main_Roles_Select.png");
	}
	else if (!m_isHovered && m_isActive)
	{
		m_bitmapRender->ChangeTexture("T_Main_Roles.png");
	}
}



void ExitButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		PostQuitMessage(0);
		m_isPressed = false;
	}

	if (m_isHovered && m_isActive)
	{
		m_bitmapRender->ChangeTexture("T_Main_Exit_Select.png");
	}
	else if (!m_isHovered && m_isActive)
	{
		m_bitmapRender->ChangeTexture("T_Main_Exit.png");
	}
}

void ResetStageButton::SetWhiteStonePos(vector<POINT> whiteStonePos)
{
	m_whiteStonePos = whiteStonePos;
	m_blackStoneCount = m_boardManager.m_playerInfo.m_BlackStone; // 현재 흑돌 개수
	m_money = m_boardManager.m_playerInfo.m_money; // 현재 돈
}

void ResetStageButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		m_boardManager.ResetStone();
		
		for (POINT& pos : m_whiteStonePos) m_boardManager.PlaceStone(pos, StoneType::White, StoneAbility::None);
		m_boardManager.m_playerInfo.m_BlackStone = m_blackStoneCount; // 흑돌 개수 초기화
		m_boardManager.m_playerInfo.m_money = m_money; // 돈 초기화

		m_isPressed = false;
	}

	if (m_isHovered && m_isActive)
	{
		m_bitmapRender->ChangeTexture("T_Common_1Reset_Selcet.png");
	}
	else if (!m_isHovered && m_isActive)
	{
		m_bitmapRender->ChangeTexture("T_Common_1Reset.png");
	}
}

void NextButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		std::cout << "index increase" << std::endl;
		refIndex++;
		m_isPressed = false;
	}


}

void PrevButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		std::cout << "index decrease" << std::endl;
		refIndex--;
		m_isPressed = false;
	}
}
